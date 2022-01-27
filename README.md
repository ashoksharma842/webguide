# webguide
This is simple webguide project with minimal functions
This project uses ESP32 on Arduino IDE.
A 2.8 TFT SPI 240x320 v1.2 LCD is attached to the ESP32 for taking inputs.
LCD Connections:
ESP32--------LCD
(MOSI)D23 -> 6(SDI) & 12(TDIN)
(TCS) D21 -> 11(TCS)
(MISO)D19 -> 9(SDO) & 13(TDO)
(SCK) D18 -> 7(SCK) & 10(T_CLK)
(RST)  D4 -> 14(RESET)
(DC)   D2 -> 5(DC)
(CS)  D15 -> 3(CS)

Sensor Connections:
Sensor------ESP32:ADC (0-3.3V)
feedback -> D34
nmr      -> D35
s1       -> D32
s2       -> D33
current  -> D25

Motor Connections:
ESP32-------Motor driver
D14      -> PWM
D27      -> INB
D26      -> INA
D25      -> CS (current sensor : ADC)
