#include <Arduino.h>
#include <AccelStepper.h>

#ifdef TARGET_ESP32
	#define EN_PIN           26 // Enable
	#define STEP_PIN_X       27  // Step x 
	#define DIR_PIN_X        14  // Direction x
	#define STEP_PIN_Y       12  // Step y
	#define DIR_PIN_Y        13  // Direction y

	#define SW_PIN_X		32
	#define SW_PIN_Y		33

	#define SENSOR_Pin		25	//Glass sensor 
#endif
#ifdef TARGET_NANO
	#define EN_PIN           6 // Enable
	#define STEP_PIN_X       5  // Step x 
	#define DIR_PIN_X        4  // Direction x
	#define STEP_PIN_Y       3  // Step y
	#define DIR_PIN_Y        2  // Direction y
	
	#define SW_PIN_X		7
	#define SW_PIN_Y		8
#endif

AccelStepper stepper_X(AccelStepper::DRIVER,STEP_PIN_X,DIR_PIN_X);
AccelStepper stepper_Y(AccelStepper::DRIVER,STEP_PIN_Y,DIR_PIN_Y);

							//(steps per rotation*micro stepping)/(toothCount*toothSeperation)
constexpr uint32_t steps_per_mm_X = (200*8)/(20*2);//=
constexpr uint32_t steps_per_mm_Y = (200*8)/(30*2);//=

bool dir = true;

int delayTime = 50;

const int posGap = 47;//mm

void X_moveTo(float mm)
{
	stepper_X.moveTo((int)mm*steps_per_mm_X);
}
float X_currentPosition()
{
	return (float)stepper_X.currentPosition()/steps_per_mm_X;
}
void Y_moveTo(float mm)
{	
	stepper_Y.moveTo((long)mm*steps_per_mm_Y);
}
float Y_currentPosition()
{
	return (float)stepper_Y.currentPosition()/steps_per_mm_Y;
}
void move_home()
{
	Serial.println("Move Home");
	Serial.println(X_currentPosition());
	Serial.println(Y_currentPosition());

	stepper_X.moveTo(0);
	stepper_Y.moveTo(0);
	while((stepper_X.distanceToGo()!=0)||(stepper_Y.distanceToGo()!=0))
	{
		if (stepper_X.distanceToGo()!=0)
		{
			stepper_X.run();
		}		

		if (stepper_Y.distanceToGo()!=0)
		{
			stepper_Y.run();
		}
		//delay(2);

	}
	Serial.println("On Home");
	
}
void sense_home()
{	
	Serial.println("Sense Home");

	stepper_X.setSpeed(-5*steps_per_mm_X);
	stepper_Y.setSpeed(5*steps_per_mm_Y);

	stepper_Y.move(2000*steps_per_mm_Y);
	while (digitalRead(SW_PIN_Y))
	{
		stepper_Y.run();
		delay(1);
	}
	stepper_Y.setCurrentPosition(135*steps_per_mm_Y);
	
	stepper_X.move(-2000*steps_per_mm_X);
	while (digitalRead(SW_PIN_X))
	{
		stepper_X.run();
		delay(1);
	}
	stepper_X.setCurrentPosition(-15*steps_per_mm_X);

	stepper_X.setSpeed(50*steps_per_mm_X);
	stepper_Y.setSpeed(50*steps_per_mm_Y);

	move_home();
}
void stepper_on()
{
	digitalWrite(EN_PIN, LOW);
}
void stepper_off()
{
	digitalWrite(EN_PIN, HIGH);
}

const uint8_t Xcnt = 6;
const uint8_t Ycnt = 4;

int posY_(uint8_t id){
	return (int)id/(int)Xcnt;
}
int posY(uint8_t id){
	return posGap*posY_(id);
}
int posX(uint8_t id){

	return posGap*(((posY_(id)%2)==0)	?
		(id-(Xcnt)*posY_(id))	:	((1+posY_(id))*Xcnt-1-id));

}
void movePos(uint8_t id){

	X_moveTo(posX(id));
	Y_moveTo(posY(id));
}
void movePosFill(uint8_t id){
	X_moveTo(posX(id));
	Y_moveTo(posY(id)+20);
}

void setup(){
	#ifdef TARGET_ESP32
		Serial.begin(115200);
	#endif
	#ifdef TARGET_NANO
		Serial.begin(115200);
	#endif

	while(!Serial);
	Serial.println("Start...");

	pinMode(SW_PIN_X, INPUT);
	pinMode(SW_PIN_Y, INPUT);

	pinMode(EN_PIN, OUTPUT);
	digitalWrite(EN_PIN, HIGH); //deactivate driver (LOW active)

	#define speedMultiplier 200		// 100mm/s @ 80 steps/mm
	#define accelMultiplier 1000	// 2000mm/s^2

	stepper_X.setMaxSpeed(speedMultiplier*steps_per_mm_X); 
	stepper_X.setAcceleration(accelMultiplier*steps_per_mm_X);
	stepper_X.setPinsInverted(true,false,true);
	stepper_X.setMinPulseWidth(100);
	

	stepper_Y.setMaxSpeed(speedMultiplier*steps_per_mm_Y); // 100mm/s @ 80 steps/mm
	stepper_Y.setAcceleration(accelMultiplier*steps_per_mm_Y); // 2000mm/s^2
	stepper_Y.setPinsInverted(false,false,true);
	stepper_Y.setMinPulseWidth(100);
	
		
	digitalWrite(EN_PIN, LOW); //activate driver
	
	//stepper_X.move(50*steps_per_mm);
	stepper_X.setSpeed(50*steps_per_mm_X);
	//stepper_X.runSpeedToPosition();
	//stepper_X.runToPosition();
	
	//stepper_Y.move(50*steps_per_mm);
	stepper_Y.setSpeed(50*steps_per_mm_Y);
	//stepper_Y.runSpeedToPosition();
	//stepper_Y.runToPosition();
	
	sense_home();

	Serial.println("end Setup");
}

String serialString = "";

int state = 0;

unsigned long loopCounter = 0;
unsigned long  loopTimer = 0;

void loop() {

	if(Serial.available())
	{
		while(Serial.available()) 
		{
			serialString= Serial.readString();// read the incoming data as string
			Serial.println(serialString);
		}
		if (serialString.charAt(0)=='Y')
		{	
			Serial.print("Moving Y to ");
			Serial.println(serialString.substring(1));

			stepper_Y.moveTo(serialString.substring(1).toInt()*steps_per_mm_Y);
			
		}else if (serialString.charAt(0)=='X')
		{	
			Serial.print("Moving X to ");
			Serial.println(serialString.substring(1));
			
			stepper_X.moveTo(serialString.substring(1).toInt()*steps_per_mm_X);
			
		}else if (serialString.charAt(0)=='A')
		{	
			Serial.print("Moving Test 'A' ");
			state = 1;
		}else if (serialString.charAt(0)=='B')
		{	
			Serial.print("Moving Test 'B' ");
			state = 11;
		}else if (serialString.charAt(0)=='S')
		{	
			stepper_X.setMaxSpeed((int)serialString.substring(1).toInt()*steps_per_mm_X);
			stepper_Y.setMaxSpeed((int)serialString.substring(1).toInt()*steps_per_mm_Y);
			Serial.print("changed Max Speed");
		}else if (serialString.charAt(0)=='s')
		{	
			stepper_X.setSpeed((int)serialString.substring(1).toInt()*steps_per_mm_X);
			stepper_Y.setSpeed((int)serialString.substring(1).toInt()*steps_per_mm_Y);
			Serial.print("changed Speed");
		}else if (serialString.charAt(0)=='a')
		{	
			stepper_X.setAcceleration((int)serialString.substring(1).toInt()*steps_per_mm_X);
			stepper_Y.setAcceleration((int)serialString.substring(1).toInt()*steps_per_mm_Y);
			Serial.print("Moving Test 'B' ");
		}else if (serialString.charAt(0)=='T')
		{
			Serial.println(analogRead(SENSOR_Pin));
			move_home();
		}else{
			Serial.println("else");
			uint8_t id = (int)serialString.toInt();
			Serial.printf("id=%d",id);
			if (id<(Xcnt*Ycnt-1))
				movePos(id);
		}

	}

	if((stepper_X.distanceToGo()==0)&&(stepper_Y.distanceToGo()==0))
	{	
		//Serial.print("To go is 0A");
		switch (state)
		{	
		case 1:
			X_moveTo(350);
			state++;
			break;
		case 2:
			Y_moveTo(150);
			state++;
			break;
		case 3:
			X_moveTo(0);
			state++;
			break;
		case 4:
			Y_moveTo(0);
			state++;
			break;
		case 5:
			X_moveTo(150);
			Y_moveTo(150);
			state++;
			break;
		case 6:
			X_moveTo(300);
			Y_moveTo(0);
			state++;
			break;
		case 7:
			Y_moveTo(150);
			state++;
			break;
		case 8:
			X_moveTo(150);
			Y_moveTo(0);
			state++;
			break;
		case 9:
			X_moveTo(0);
			Y_moveTo(150);
			state++;
			break;
		case 10:			
			Y_moveTo(0);
			state = 0;
			break;
		default:
			break;
		}
	
		//Serial.print("To go is 0A");
		switch (state)
		{	
		case 11:
			X_moveTo(0);
			Y_moveTo(30);
			state++;
			break;
		case 12:
			X_moveTo(0);
			Y_moveTo(30+posGap);
			state++;
			break;
		case 13:
			X_moveTo(0);
			Y_moveTo(30+posGap*2);
			state++;
			break;
		case 14:
			X_moveTo(posGap*1);
			Y_moveTo(30+posGap*2);
			state++;
			break;
		case 15:
			X_moveTo(posGap*1);
			Y_moveTo(30+posGap);
			state++;
			break;
		case 16:
			X_moveTo(posGap*1);
			Y_moveTo(30);
			state++;
			break;
		case 17:
			X_moveTo(posGap*2);
			Y_moveTo(30);
			state++;
			break;
		case 18:
			X_moveTo(posGap*2);
			Y_moveTo(30+posGap);
			state++;
			break;
		case 19:
			X_moveTo(posGap*2);
			Y_moveTo(30+posGap*2);
			state++;
			break;
		case 20:
			X_moveTo(posGap*3);
			Y_moveTo(30+posGap*2);
			state++;
			break;
		case 21:
			X_moveTo(posGap*3);
			Y_moveTo(30+posGap);
			state++;
			break;
		case 22:
			X_moveTo(posGap*3);
			Y_moveTo(30);
			state++;
			break;
		case 23:
			X_moveTo(posGap*4);
			Y_moveTo(30);
			state++;
			break;
		case 24:
			X_moveTo(posGap*4);
			Y_moveTo(30+posGap);
			state++;
			break;
		case 25:
			X_moveTo(posGap*4);
			Y_moveTo(30+posGap*2);
			state++;
			break;
		case 26:
			X_moveTo(0);
			Y_moveTo(0);
			state = 0;
			break;
		default:
			break;
		}

	loopCounter++;
	if (millis()-loopTimer>1000)
	{	
		loopTimer = millis();
		Serial.print("Loops per Seccond: ");
		Serial.println(loopCounter);
		loopCounter = 0;
	}
		

	//Stepper runner:		
	if (stepper_X.distanceToGo()!=0)
	{
		stepper_X.run();
	}

	if (stepper_Y.distanceToGo()!=0)
	{
		stepper_Y.run();
	}
}