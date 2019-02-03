#include "Fake_WiringPi.h"

int wiringPiSetup (void)
{
	return 0;
}

void pinMode (int pin, int mode)
{

}

int digitalRead (int pin)
{
	return 0;
}

void digitalWrite (int pin, int value)
{

}

int waitForInterrupt (int pin, int mS)
{
	return 0;
}

void pullUpDnControl (int pin, int pud)
{

}

int wiringPiISR (int pin, int mode, void (*function)(void))
{
	return 0;
}


/*
extern int wiringPiFailure (int fatal, const char *message, ...);

struct wiringPiNodeStruct *wiringPiFindNode (int pin);
struct wiringPiNodeStruct *wiringPiNewNode (int pinBase, int numPins);

void wiringPiVersion (int *major, int *minor);
int  wiringPiSetupSys (void);
int  wiringPiSetupGpio (void);
int  wiringPiSetupPhys (void);

void pinModeAlt (int pin, int mode);
unsigned int digitalRead8 (int pin);
void digitalWrite8 (int pin, int value);
void pwmWrite (int pin, int value);
int  analogRead (int pin);
void analogWrite (int pin, int value);
int  wiringPiSetupPiFace (void);
int  wiringPiSetupPiFaceForGpioProg (void);	
int piGpioLayout (void);
int piBoardRev (void);	// Deprecated
void piBoardId (int *model, int *rev, int *mem, int *maker, int *overVolted);
int wpiPinToGpio (int wpiPin);
int physPinToGpio (int physPin);
void setPadDrive (int group, int value);
int getAlt (int pin);
void pwmToneWrite (int pin, int freq);
void pwmSetMode (int mode);
void pwmSetRange (unsigned int range);
void pwmSetClock (int divisor);
void gpioClockSet (int pin, int freq);
unsigned int digitalReadByte (void);
unsigned int digitalReadByte2 (void);
void digitalWriteByte (int value);
void digitalWriteByte2 (int value);
int wiringPiISR (int pin, int mode, void (*function)(void));
int piThreadCreate (void *(*fn)(void *));
void piLock (int key);
void piUnlock (int key);
int piHiPri (const int pri);
void delay (unsigned int howLong);
void delayMicroseconds (unsigned int howLong);
unsigned int millis (void);
unsigned int micros (void);
*/