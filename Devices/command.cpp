#include "command.h"
#include "Arduino.h"
#include "communication.h"

/**
	The incoming command is executed here by a switch case.

	#param cmd[] The data from the message
*/

Communication communication;

void Command::executeCommand(uint8_t cmd[]){
  Serial.print("Command is: ");                                                        
  switch (cmd[2]){    
	case 1:
	      powerDevice(cmd[4], cmd[3]); 
	      break;
    case 2:
	      Serial.println("Dim the device");
	      dimDevice(3, cmd[3]);
	      break;
    case 3:
	      Serial.println("RGB");
	      getRGB(cmd);
	      fadeLamp(3);
	      break;
    case 4:
	      Serial.println("Powering Senseo");
	      powerDevice(cmd[4], SENSEO);						
	      break;
    case 5:
		if(flagReadySenseo){
	        powerDevice(cmd[4], cmd[3]);
        }else if (!flagReadySenseo && flagWaitSenseo){
        	Serial.println("Not warm yet");
	    }else if (!flagReadySenseo && !flagWaitSenseo){
    		Serial.println("Senseo is not turned on");
	    }
		break;
	case 6:
		Serial.println("Update LED strip");
		communication.ledStripActive = ledStripState = cmd[3];
		getRGB(cmd);
		ledStrip();
	case 7:
		Serial.println("Temperature: ");
		Serial.println(analogRead(TEMP_SENSOR));
		cmd[3] = analogRead(TEMP_SENSOR);
		communication.prepareMessage(cmd, true);
		communication.state = 4;
		break;
    default:
		Serial.println("Command not found --> ");
		Serial.println(cmd[2]);
		break;
  }
}

/**
	Every device which can be powered is listed below. some device only need a short moment of current and flick off after a second
	Also powers the buttons of the Senseo

	@param value This indicates if the device should be turned on or off
	@param deviceNr this indicates which device should be powered
*/

void Command::getRGB(uint8_t cmd[]){
	PWM[0] = cmd[3];
  	PWM[1] = cmd[4];
  	PWM[2] = cmd[5];
}

void Command::powerDevice(uint8_t value, uint8_t deviceNr){
	switch(deviceNr){
		case SENSEO:
			PORTD = value ? (PORTD | (1 << BLUE_RGB_PIN)) : (PORTD & ~(1 << BLUE_RGB_PIN));
	  		Serial.println("SENSEO");
	  		flagWaitSenseo = value;
	  		delay(1000);
	  		PORTD &= ~(1 << BLUE_RGB_PIN);
	  		break;
	  	case LAMP:
	  		analogWrite(PWM_PIN, value * 255);
  			Serial.println("LAMP");
	  		break;   
		case CUP_1:
	  		Serial.println("Cup_1");
	  		PORTD |= (1 << GREEN_RGB_PIN);
	  		delay(1000);
	  		PORTD &= ~(1 << GREEN_RGB_PIN);
			break;
		case CUP_2:
	 		Serial.println("Cup_2");
	  		PORTD |= (1 << 3);
	  		delay(1000);
	  		PORTD &= ~(1 << 3);
			break;
		case LAVA:
			Serial.println("Lava lamp");
			PORTD = value ? (PORTD | (1 << LAVA_LAMP_PIN)) : (PORTD & ~(1 << LAVA_LAMP_PIN));
  			break;
  		case WATER_BOILER:
			Serial.println("Water boiler");
  			PORTD = value ? (PORTD | (1 << BOILER_PIN)) : (PORTD & ~(1 << BOILER_PIN));
  			break;
  		// case NEW_DEVICE:
		// 	add new device here
		// 	break;
		default:
	  		Serial.print("Unknown device nr: ");
	  		Serial.println(deviceNr);
			break;
	}	
}

void Command::ledStrip(){
	switch(ledStripState){
		case 1:
			Serial.println("Ambient light");
			color();
			Serial.print("R Intensity:");  
		    Serial.print(PWM[0], DEC);  
		    Serial.print(" G Intensity: ");  
		    Serial.print(PWM[1], DEC);  
		    Serial.print(" B Intensity : ");  
		    Serial.print(PWM[2], DEC);
	        fadeLamp(0);
			break;
		case 2:
			Serial.println("Changing light");
			fadeLamp(0);
			break;
		case 3:
			Serial.println("Nunchuck");
			readNunchuck();
			fadeLamp(0);
			break;
		case 4:
		  	Serial.println("fading effect");
		  	int r, g, b, fadeSpeed = 5;
			for (r = 0; r < 256; r++) { 
				analogWrite(RED_RGB_PIN, r);
				delay(fadeSpeed);
			} 
			for (b = 255; b > 0; b--) { 
				analogWrite(BLUE_RGB_PIN, b);
				delay(fadeSpeed);
			} 
			for (g = 0; g < 256; g++) { 
				analogWrite(GREEN_RGB_PIN, g);
				delay(fadeSpeed);
			} 
			for (r = 255; r > 0; r--) { 
				analogWrite(RED_RGB_PIN, r);
				delay(fadeSpeed);
			} 
			for (b = 0; b < 256; b++) { 
				analogWrite(BLUE_RGB_PIN, b);
				delay(fadeSpeed);
			} 
			for (g = 255; g > 0; g--) { 
				analogWrite(GREEN_RGB_PIN, g);
				delay(fadeSpeed);
			} 
			break;
		default:
			break;
	}
}

/*
	Reads the current color values from the sensor
*/
void Command::readNunchuck(){

}

void Command::color(){    
  digitalWrite(S2, LOW);  
  digitalWrite(S3, LOW);  
  //count OUT, pRed, RED  
  PWM[0] = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);  
  digitalWrite(S3, HIGH);  
  //count OUT, pBLUE, BLUE  
  PWM[1] = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);  
  digitalWrite(S2, HIGH);  
  //count OUT, pGreen, GREEN  
  PWM[2] = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);  
}

/**
	Reads the current PWM and slowly fade to the new PWM
	
	@param delayTime - the time it should take to get the lamp to the new value
	@param value - the brightness you want the lamp to be
*/
void Command::dimDevice(uint8_t delayTime, uint8_t value){
	uint16_t secCount = 1;
	
	for (int counter = 0; counter <= (1000 * delayTime); ++counter){
		secCount = pwmColour(PWMM, value, counter, secCount, delayTime);
		_delay_ms(1);
	}
	analogWrite(deviceState.pin[PWMM], value);
	deviceState.RGB[PWMM] = value;
}

/**
	Reads the current PWM and slowly changes to the new PWM for each color
	
	@param delayTime - the time it should take to get the lamp to the new value
	@param newValue[] - the color you want the RGB lamp to be
*/
void Command::fadeLamp(uint8_t delayTime){
	uint16_t secCount[3]={1,1,1};
	for (int counter = 0; counter <= (1000 * delayTime); ++counter){
		secCount[RED] = pwmColour(RED, PWM[RED], counter, secCount[RED],delayTime);
		secCount[GREEN] = pwmColour(GREEN, PWM[GREEN], counter, secCount[GREEN],delayTime);
		secCount[BLUE] = pwmColour(BLUE, PWM[BLUE], counter, secCount[BLUE],delayTime);
		_delay_ms(1);
	}
	deviceState.RGB[RED] = PWM[RED];
	deviceState.RGB[GREEN] = PWM[GREEN];
	deviceState.RGB[BLUE] = PWM[BLUE];
}
/**
	For every counter this function will decrease or increase the PWM of a device. The time it takes is given by delayTime

	@param colour The colour/pin of the lamp (these are defined)
	@param newValue The value of the pin that it should reach
	@param counter An internal counter to know how far
	@param secCount the previous count value, this is returned
	@param delayTime the time it should take to reach the new colour
*/
uint16_t Command::pwmColour(uint8_t colour, uint8_t newValue, int counter, uint16_t secCount, uint8_t delayTime){
	uint8_t calc;
	uint8_t dif = abs(deviceState.RGB[colour] - newValue);
	uint16_t amount = (1000 * delayTime)/ dif;
		
	if (counter == (amount * secCount) && secCount <= dif){
		if (deviceState.RGB[colour] > newValue){
		  calc = (newValue + (dif - secCount));
		}else if (deviceState.RGB[colour] < newValue){
		  calc = (newValue - (dif - secCount));
		}
		analogWrite(deviceState.pin[colour], calc);
		secCount += 1;
	}
	return secCount;
}

/**
	Reads the Senseo and changes the flag as soon as the Senseo is warmed up
	Every time the senseo led is on, the function registers for how long
	if it stays on for a longer than a second, the led will	stay on meaning 
	its finished warming up
*/
bool Command::readSenseo(){
	if (!timerSet){
		counter = 0;					
		timer = millis();
		timerSet = true;
	}
	if (!(PIND & 0b00000100) && (counter < SENSEO_COUNT)){
		counter++;
		_delay_ms(1);
	}else if ((PIND & 0b00000100)) {
		timerSet = false;
		flagReadySenseo = false;
	}
	Serial.println(counter);
	if ((counter >= SENSEO_COUNT) && (millis() > timer + 1000) && timerSet){
		Serial.println("Done");
		timerSet = false;
		flagWaitSenseo = false;
		_delay_ms(1);
		flagReadySenseo = true;
	}else if ((counter < SENSEO_COUNT) && (millis() > timer + 1000) && timerSet){
		timerSet = false;
		// return false;
	}
}

/**
	The doorbell and senseo are checked every now and then
	whenever one of them has a notable change it will set a flag true
*/
void Command::checkFlags(){
	if (flagWaitSenseo){
		readSenseo();
		flagWaitSenseo != flagReadySenseo;
	}

	if(nunchuckInputButton && !debounce){
		ledStrip();
	}else if(!nunchuckInputButton){
		debounce = false;
	}

	if ((PIND & 0b10000000) && !debounce){
  		flagLED = true;
		debounce = true;
		flagDoorBell = true;
	}else if (!(PIND & 0b10000000)){
		debounce = false;
	}
}	
