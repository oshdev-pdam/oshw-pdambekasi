EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:ac-dc
LIBS:Altera
LIBS:analog_devices
LIBS:Battery_Management
LIBS:bbd
LIBS:bosch
LIBS:brooktre
LIBS:dc-dc
LIBS:Diode
LIBS:Display
LIBS:elec-unifil
LIBS:ESD_Protection
LIBS:ftdi
LIBS:gennum
LIBS:hc11
LIBS:infineon
LIBS:intersil
LIBS:ir
LIBS:Lattice
LIBS:LEM
LIBS:logic_programmable
LIBS:maxim
LIBS:Mechanical
LIBS:modules
LIBS:motor_drivers
LIBS:nordicsemi
LIBS:nxp
LIBS:onsemi
LIBS:Oscillators
LIBS:Power_Management
LIBS:powerint
LIBS:pspice
LIBS:references
LIBS:rfcom
LIBS:RFSolutions
LIBS:sensors
LIBS:silabs
LIBS:supertex
LIBS:triac_thyristor
LIBS:video
LIBS:wiznet
LIBS:Worldsemi
LIBS:Xicor
LIBS:zetex
LIBS:Zilog
LIBS:Sensors
LIBS:Project 1-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ADAFRUIT_FEATHER U1
U 2 1 59EE9F45
P 5950 3450
F 0 "U1" V 6150 2900 60  0000 C CNN
F 1 "ADAFRUIT_FEATHER" V 6150 3700 60  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x16_Pitch2.54mm" H 6100 3250 60  0000 C CNN
F 3 "" H 6100 3250 60  0000 C CNN
	2    5950 3450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 59EEA0B4
P 4550 3850
F 0 "#PWR01" H 4550 3600 50  0001 C CNN
F 1 "GND" H 4550 3700 50  0000 C CNN
F 2 "" H 4550 3850 50  0001 C CNN
F 3 "" H 4550 3850 50  0001 C CNN
	1    4550 3850
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR02
U 1 1 59EEA0E4
P 5700 2900
F 0 "#PWR02" H 5700 2750 50  0001 C CNN
F 1 "VCC" H 5700 3050 50  0000 C CNN
F 2 "" H 5700 2900 50  0001 C CNN
F 3 "" H 5700 2900 50  0001 C CNN
	1    5700 2900
	0    -1   -1   0   
$EndComp
Text GLabel 4550 3650 0    60   Input ~ 0
TX
Text GLabel 4550 3750 0    60   Input ~ 0
RX
Text GLabel 6600 3150 1    60   Input ~ 0
TX
Text GLabel 6600 3250 3    60   Input ~ 0
RX
$Comp
L CAP_POL C1
U 1 1 59EEAACE
P 7250 2900
F 0 "C1" H 7250 2970 60  0000 R CNN
F 1 "CAP_POL" H 7250 2820 60  0000 R CNN
F 2 "Capacitors_THT:CP_Radial_D13.0mm_P5.00mm" V 6990 2800 60  0001 C CNN
F 3 "" V 7090 2900 60  0001 C CNN
	1    7250 2900
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02 J1
U 1 1 59F55153
P 6800 3150
F 0 "J1" H 6800 3250 50  0000 C CNN
F 1 "SIM900" H 6800 2950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 6800 3150 50  0001 C CNN
F 3 "" H 6800 3150 50  0001 C CNN
	1    6800 3150
	1    0    0    -1  
$EndComp
Text GLabel 7050 4050 0    60   Input ~ 0
SDA
Text GLabel 7050 4250 0    60   Input ~ 0
SCL
Text GLabel 5600 4000 0    60   Input ~ 0
SDA
Text GLabel 5600 3900 0    60   Input ~ 0
SCL
$Comp
L Conn_01x02 J2
U 1 1 59F55349
P 7300 4100
F 0 "J2" H 7300 4200 50  0000 C CNN
F 1 "RTC" H 7300 3900 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 7300 4100 50  0001 C CNN
F 3 "" H 7300 4100 50  0001 C CNN
	1    7300 4100
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x04 J3
U 1 1 59F55471
P 7900 3700
F 0 "J3" H 7900 3900 50  0000 C CNN
F 1 "Micro Pro" H 7900 3400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 7900 3700 50  0001 C CNN
F 3 "" H 7900 3700 50  0001 C CNN
	1    7900 3700
	1    0    0    -1  
$EndComp
Text GLabel 7700 3600 0    60   Input ~ 0
DI
Text GLabel 7700 3700 0    60   Input ~ 0
DE
Text GLabel 7700 3800 0    60   Input ~ 0
RE
Text GLabel 7700 3900 0    60   Input ~ 0
RO
Text GLabel 5700 3200 0    60   Input ~ 0
DI
Text GLabel 5700 3300 0    60   Input ~ 0
DE
Text GLabel 5700 3400 0    60   Input ~ 0
RE
Text GLabel 5700 3500 0    60   Input ~ 0
RO
$Comp
L Jack-DC J5
U 1 1 59F5568F
P 8000 2900
F 0 "J5" H 8000 3110 50  0000 C CNN
F 1 "Jack-DC" H 8000 2725 50  0000 C CNN
F 2 "Connectors:BARREL_JACK" H 8050 2860 50  0001 C CNN
F 3 "" H 8050 2860 50  0001 C CNN
	1    8000 2900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 59F55DFB
P 8400 3000
F 0 "#PWR03" H 8400 2750 50  0001 C CNN
F 1 "GND" H 8400 2850 50  0000 C CNN
F 2 "" H 8400 3000 50  0001 C CNN
F 3 "" H 8400 3000 50  0001 C CNN
	1    8400 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3900 5700 3900
Wire Wire Line
	5600 4000 5700 4000
Wire Wire Line
	7050 4050 7100 4050
Wire Wire Line
	7100 4050 7100 4100
Wire Wire Line
	7050 4250 7100 4250
Wire Wire Line
	7100 4250 7100 4200
Wire Wire Line
	8400 3000 8300 3000
Wire Wire Line
	8300 3000 8300 2900
Wire Wire Line
	8300 2800 8450 2800
$Comp
L GND #PWR04
U 1 1 59F55F5F
P 7250 3150
F 0 "#PWR04" H 7250 2900 50  0001 C CNN
F 1 "GND" H 7250 3000 50  0000 C CNN
F 2 "" H 7250 3150 50  0001 C CNN
F 3 "" H 7250 3150 50  0001 C CNN
	1    7250 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 2800 8450 2750
$Comp
L VCC #PWR05
U 1 1 59F589FE
P 8450 2750
F 0 "#PWR05" H 8450 2600 50  0001 C CNN
F 1 "VCC" H 8450 2900 50  0000 C CNN
F 2 "" H 8450 2750 50  0001 C CNN
F 3 "" H 8450 2750 50  0001 C CNN
	1    8450 2750
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR06
U 1 1 59F58A27
P 7250 2650
F 0 "#PWR06" H 7250 2500 50  0001 C CNN
F 1 "VCC" H 7250 2800 50  0000 C CNN
F 2 "" H 7250 2650 50  0001 C CNN
F 3 "" H 7250 2650 50  0001 C CNN
	1    7250 2650
	1    0    0    -1  
$EndComp
$Comp
L Screw_Terminal_01x02 J4
U 1 1 59F58CD3
P 7950 4350
F 0 "J4" H 7950 4450 50  0000 C CNN
F 1 "solid state controller" H 7950 4150 50  0000 C CNN
F 2 "TerminalBlocks_Phoenix:TerminalBlock_Phoenix_PT-3.5mm_2pol" H 7950 4350 50  0001 C CNN
F 3 "" H 7950 4350 50  0001 C CNN
	1    7950 4350
	-1   0    0    1   
$EndComp
Text GLabel 5700 3700 0    60   Input ~ 0
SS
Text GLabel 8250 4250 2    60   Input ~ 0
SS
Wire Wire Line
	8150 4250 8250 4250
$Comp
L GND #PWR07
U 1 1 59F58EEA
P 8200 4350
F 0 "#PWR07" H 8200 4100 50  0001 C CNN
F 1 "GND" H 8200 4200 50  0000 C CNN
F 2 "" H 8200 4350 50  0001 C CNN
F 3 "" H 8200 4350 50  0001 C CNN
	1    8200 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 4350 8150 4350
$Comp
L Screw_Terminal_01x02 J6
U 1 1 59F5901B
P 8350 3600
F 0 "J6" H 8350 3700 50  0000 C CNN
F 1 "Battery" H 8350 3400 50  0000 C CNN
F 2 "TerminalBlocks_Phoenix:TerminalBlock_Phoenix_PT-3.5mm_2pol" H 8350 3600 50  0001 C CNN
F 3 "" H 8350 3600 50  0001 C CNN
	1    8350 3600
	-1   0    0    1   
$EndComp
$Comp
L +BATT #PWR08
U 1 1 59F59158
P 8850 3500
F 0 "#PWR08" H 8850 3350 50  0001 C CNN
F 1 "+BATT" H 8850 3640 50  0000 C CNN
F 2 "" H 8850 3500 50  0001 C CNN
F 3 "" H 8850 3500 50  0001 C CNN
	1    8850 3500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 59F59187
P 8700 3650
F 0 "#PWR09" H 8700 3400 50  0001 C CNN
F 1 "GND" H 8700 3500 50  0000 C CNN
F 2 "" H 8700 3650 50  0001 C CNN
F 3 "" H 8700 3650 50  0001 C CNN
	1    8700 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 3500 8850 3500
Wire Wire Line
	8550 3600 8700 3600
Wire Wire Line
	8700 3600 8700 3650
$Comp
L VCC #PWR010
U 1 1 59F5B64D
P 4450 2750
F 0 "#PWR010" H 4450 2600 50  0001 C CNN
F 1 "VCC" H 4450 2900 50  0000 C CNN
F 2 "" H 4450 2750 50  0001 C CNN
F 3 "" H 4450 2750 50  0001 C CNN
	1    4450 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 2750 4600 2750
$Comp
L ADAFRUIT_FEATHER U2
U 1 1 59F6A2D9
P 4800 3300
F 0 "U2" V 5000 2750 60  0000 C CNN
F 1 "ADAFRUIT_FEATHER" V 5000 3550 60  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x12_Pitch2.54mm" H 4950 3100 60  0000 C CNN
F 3 "" H 4950 3100 60  0000 C CNN
	1    4800 3300
	1    0    0    -1  
$EndComp
Connection ~ 4550 2750
Wire Bus Line
	7250 2700 7250 2650
Wire Bus Line
	7250 3100 7250 3150
$EndSCHEMATC
