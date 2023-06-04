#ifndef PIN_CONFIG_H /* include guards */
#define PIN_CONFIG_H

//*** Pin Config
// Stepper
#define EN_PIN 16      // Enable
#define STEP_PIN_X 19  // Step x
#define DIR_PIN_X 18   // Direction x
#define STEP_PIN_Y 5   // Step y
#define DIR_PIN_Y 17   // Direction y

// EndStops
#define SW_PIN_X 14  // End Stop X
#define SW_PIN_Y 27  // End Stop Y

// Sensors
#define SENSOR_Pin 13  // Glass sensor

// Pump
#define PUMP_A_A 25 
#define PUMP_A_B 26

#define PUMP_B_A 32
#define PUMP_B_B 33

//Buttons
#define Button1 36
#define Button2 39
#define Button3 34
#define Button4 35

// LED
#define WS2812_Pin 23

#endif /* PIN_CONFIG_H */