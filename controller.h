/**
* Module : controller.h
* Project : Web Guide Controller
* Originated : 2021.11.12
*
* Overview : definations required for controlling of web guide.
*
*/
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#define ADC_CHANNELS  5
#define ADC_MIDVAL  2048
#define ADC_MAXVAL  4095
#define ADC_MINVAL  0

extern const int sensor1Pin;
extern const int sensor2Pin;
extern const int feedbackPin;
extern const int namurPin;
extern const int currentSensePin;

extern const int motor1Pin1, motor1Pin2;
extern volatile uint8_t gain;
extern const int pwmChannel;

extern volatile int* edgeData;
extern volatile int* feedbackData;
extern volatile int* refEdgeData;
extern volatile int* refFeedbackData;
extern volatile int** sensorData;
extern volatile int** referenceData;

#define MANUAL  0
#define AUTO  1
#define SC    2

#define SENSOR1 1
#define SENSOR2 2
#define CENTER_LINE 3

//Guiding Mode sensors
#define GM_SENSOR1 1
#define GM_SENSOR2 2
#define GM_NAMUR 3
#define GM_ACT_FB 4

#define STOP  0
#define FORWARD  1
#define BACKWARD  (-1)

#define NAMUR  0
#define ACTUATOR_FEEDBACK 1
/*****************************************************************************
* Function:    DisplayInfo
*
* Overview:   Displays CPU freq and APB freq
*
* Entry args: void
* 
* Return val: void
*/
void DisplayInfo(void);

/*****************************************************************************
* Function:    ReadAdcData
*
* Overview:   reads all the adc data.
*
* Entry args: channels[ADC_CHANNELS] :numbers of channels to be read in array
*       data[ADC_CHANNELS] :adc data corresponding to each channnel number
* Return val: void
*/
void read_adc_data(int *channels, volatile int *data);

/*****************************************************************************
* Function:    CalculateCorrection
*
* Overview:   reads sensor data and compares it with reference data and
*       gives output.
*
* Entry args: void
*
* Return val: PWM dutyCycle required for correction (0-255)
*/
int CalculateCorrection (void);

/*****************************************************************************
* Function:    MoveActuator
*
* Overview:   provides input to PWM and direction pins to L298N IC.
*
* Entry args: direction : FORWARD, BACKWARD, STOP
*       dutyCycle : 0-255 (as per code requrement);
* Return val: void
*/
void MoveActuator(int direction, uint8_t dutyCycle);

/*****************************************************************************
* Function:    ApplyCorrection
*
* Overview:   moves the actuator in particualar direction as per correction.
*
* Entry args: required_correction : 0-255 PWM dutyCycle
*
* Return val: motor status (FORWARD, BACKWARD, STOP)
*/
int ApplyCorrection(int requiredCorrection);
#endif //CONTROLLER_H
