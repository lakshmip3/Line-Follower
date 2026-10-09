 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"

// don't change these:

#pragma warning disable 520  //  240403 disable the " <function> is never called" warnings during compilation


/*  Peripheral Usages:

 TIMER0:   4.096mS    250Hz timebase
 
 TIMER1:   1.024mS   1000Hz  - unused for now
 
 PWM1:  Right Motor (looking forward)

 PWM2:  Left Motor  (looking forward)
  
 ADC:  Polled operation for reading POTs, not interrupt driven
 
 UART:  Serial comm, interrupt driven TX and RX for user commands
 
 */



/*

Development DID List:
--------------------------
Macro Pin definitions
LED on and off
Switch sense
Timer0 interrupt at 100Hz
Serial shifter - turn on OPTOs
Serial Tx and Rx using interrupts
Read Optos, report status at 2Hz over serial
ADC for pots and report over serial
PWM for motors forward
Use POT to change motor speed
Port change to drive motors backward or forward
LED2 follows Sensor4 
Opto IR LEDs powered  Center, All, Wider, Narrower


260912  00.21   More intuitive motor control for POT vs serial command controls, Grok review
260911  00.20   Close to release .. demoed and installed in all 11 systems
260911  00.18   Opto IR LEDs powered: Center, All, Wider, Narrower
260910  00.17   LED2 follows IR Sensor4 
260909  00.16   Apply POT3 to motor pwm
260908  00.15   POTs read, calc Mean and DiffSum of 8 samples, report
260821  00.12   PWM operations work with 1024 levels of duty cycle
260711  00.01   Macro definitions; Serial shift register driver

*/

// don't change these:

#define  USE_TWO_PRIORITY_LEVEL_INTERRUPTS  1    // 1= Use 2 INT priority levels
#define  WAIT_FOR_PWM_LOAD_DONE             0    // 1= wait for reload after PWM competes - up to 16uS wait.

#define  POT_MEANS_REPORT        1
#define  POT_SUM8_REPORT         2
#define  POT_SUM8_TIMES2_REPORT  3

#define  LED_FOLLOWS_SENSOR4     0
#define  LED_TOGGLES_AT_2HZ      1
#define  LED_TOGGLES_WITH_SWITCH 2
#define  NUM_OF_LED_MODES        3   // keep this correct if you add another LED mode

#define POT1_MUX_CHAN  0         // RA0: POT1
#define POT2_MUX_CHAN  1         // RA1: POT2
#define POT3_MUX_CHAN  2         // RA2: POT3

#define VERBOSE_OFF               0
#define VERBOSE_POT_VALUES        1
#define VERBOSE_MOTOR_PWM_VALUES  2    // Also displays IR Sensor Byte
#define VERBOSE_DATARAY_DUMP      3



// ------------------------------------------
// OK to tweak these below

#define  HELLO_STRING  " Snoopy v0.22"   // Freeze 0n 260912


uint8_t VerboseMode = VERBOSE_MOTOR_PWM_VALUES;   // Info gets serially reported

#define  POT_REPORT    POT_MEANS_REPORT

uint8_t  LedMode  =    LED_FOLLOWS_SENSOR4;   // 260910

uint8_t  OptoLEDs =    0x38;    // 0x38= Center 3 IR LEDs are on.   Others:  0x10, 0x38, 0x7C, 0xFE

#define POT_DELTA_ACTION    3   //  Adc count change threshold   



// Prototypes

uint8_t  uart_tx_buffer_is_empty (void);    // is in uart1.c  MCC Generated File.  You CAN edit them, CAREFULLY.



// READ INPUT PIN MACROS  ------------------------  READ from PORTS

#define IS_SWITCH_PRESSED  (PORTAbits.RA3 == 0)

#define IR_SENSORS  (~PORTC & 0xFE)    // strip off LSBit since it is not a sensor

#define IS_SENSOR1_HIGH     (PORTCbits.RC1)
#define IS_SENSOR1_LOW      (PORTCbits.RC1 == 0)

#define IS_SENSOR2_HIGH     (PORTCbits.RC2)
#define IS_SENSOR2_LOW      (PORTCbits.RC2 == 0)

#define IS_SENSOR3_HIGH     (PORTCbits.RC3)
#define IS_SENSOR3_LOW      (PORTCbits.RC3 == 0)

#define IS_SENSOR4_HIGH     (PORTCbits.RC4)
#define IS_SENSOR4_LOW      (PORTCbits.RC4 == 0)

#define IS_SENSOR5_HIGH     (PORTCbits.RC5)
#define IS_SENSOR5_LOW      (PORTCbits.RC5 == 0)

#define IS_SENSOR6_HIGH     (PORTCbits.RC6)
#define IS_SENSOR6_LOW      (PORTCbits.RC6 == 0)

#define IS_SENSOR7_HIGH     (PORTCbits.RC7)
#define IS_SENSOR7_LOW      (PORTCbits.RC7 == 0)

#define IS_SEROUT_HIGH      (PORTAbits.RA7)        // REM:  Serial OUT from Serial to Parallel chip
#define IS_SEROUT_LOW       (PORTAbits.RA7 == 0)


// WRITE OUTPUT PORT LATCHES  (use LAT, not PORT ? avoids RMW glitches)

#define LED_ON      (LATBbits.LATB6 = 0)   // active low
#define LED_OFF     (LATBbits.LATB6 = 1)

#define IS_LED_ON   (LATBbits.LATB6 == 0)
#define IS_LED_OFF  (LATBbits.LATB6 == 1)

#define RCLK_HIGH   (LATAbits.LATA6 = 1)
#define RCLK_LOW    (LATAbits.LATA6 = 0)

#define SCLK_HIGH   (LATAbits.LATA5 = 1)
#define SCLK_LOW    (LATAbits.LATA5 = 0)

#define SERIN_HIGH  (LATAbits.LATA4 = 1)   // rem:  Serial input to the serial to parallel chip that enables optos
#define SERIN_LOW   (LATAbits.LATA4 = 0)

#define SCLRN_HIGH  (LATCbits.LATC0 = 1)
#define SCLRN_LOW   (LATCbits.LATC0 = 0)


// READ OUTPUT PORT LATCHES

#define IS_LEFT_MOT_IN1_HIGH   (LATBbits.LATB2)
#define IS_LEFT_MOT_IN1_LOW    (LATBbits.LATB2 == 0)

#define IS_LEFT_MOT_IN2_HIGH   (LATBbits.LATB3)
#define IS_LEFT_MOT_IN2_LOW    (LATBbits.LATB3 == 0)

#define IS_RIGHT_MOT_IN1_HIGH  (LATBbits.LATB0)
#define IS_RIGHT_MOT_IN1_LOW   (LATBbits.LATB0 == 0)

#define IS_RIGHT_MOT_IN2_HIGH  (LATBbits.LATB1)
#define IS_RIGHT_MOT_IN2_LOW   (LATBbits.LATB1 == 0)


uint8_t RxChar;

#define START_PWM_10BIT       0x0200   // 0x0100    260915
#define MOTOR_OFFSET_10BIT    0x0000

#define MOTOR_PWM_MAX_10BIT   0x03FF   // 3FF  max
#define MOTOR_PWM_USER_DELTA  5        // Increment or decrement by N using Faster and Slower serial commands

// uint16_t LeftMotorPwm     = START_PWM_10BIT;                   // max 0x03FF with 10 bit PWM

uint16_t LeftMotorPwm  =  START_PWM_10BIT;                  // max 0x03FF with 10 bit PWM
uint16_t RightMotorPwm = (START_PWM_10BIT + MOTOR_OFFSET_10BIT);  // max 0x03FF with 10 bit PWM

int16_t RightPwmOffset = MOTOR_OFFSET_10BIT;   // difference between left and right motor speeds
                      // Note:  this is a single point motor cal.  Might need Y = mX + B two point cal

uint16_t LastLeftMotorPwm  = START_PWM_10BIT;
uint16_t LastRightMotorPwm = START_PWM_10BIT + MOTOR_OFFSET_10BIT;   // 260909 Added 

int8_t  MovingDirection = 0;   //  0=Not Moving  >0 = Forward  <0 = Reverse

uint8_t PotsControlMotors = 1;   // 1= Pots control motors  0= Serial control with Fast and Slow commands  260909
                                 // Note:  Pot3 controls Left Motor,  Pot1 controls Right Motor - just for demo



#define  ADC_SAMPLES_TO_TOSS   4     // 260908
#define  ADC_GND_CHAN         0x3B
#define  SET_ADC_MUX_TO_GND   ADPCH = ADC_GND_CHAN; MuxIndex = ADC_GND_CHAN   // select GND to discharge ADC Sample and Hold Cap   260908 set MuxIndex here too

// 260912 added volatile
volatile uint8_t  MuxIndex = ADC_GND_CHAN;  // 0x3B;   // Ground channel   see SET_ADC_MUX_TO_GND
                                   // 0,1,2, 0x3B only
volatile uint8_t  NextMuxIndex = 0;         // For after grounding the ADC 

volatile uint16_t DataRay[10];            // [0] is AVG,  [9] is DiffSum
volatile uint8_t  DataRayIndex;        
volatile uint8_t  PendingPotToCompute   = 0;   // 0= No Compute Request,   1,2,3 = Compute that Pot channel last 8 samples

volatile uint8_t AdcHoldoffCount   = ADC_SAMPLES_TO_TOSS;   

uint8_t  ReportingDataRayForPotNum  = 0;   // One Pot only, shows all 8 samples


//                                   RA0    RA1    RA2
//                                   Pot1   Pot2   Pot3
uint16_t PotSum8Ray    [4] = { 0,   0x000, 0x222, 0x333 };
uint16_t LastPotSum8Ray[4] = { 0,   0x111, 0x222, 0x333 };
uint16_t PotMeanRay    [4] = { 0,   0x000, 0x222, 0x333 };    // 260909
uint16_t PotDiffSumRay [4] = { 0,     0,     0,     0   };


// ============================================================
// Configuration assumptions (adjust to match your project)
// ============================================================
// - System clock (FOSC) = 64 MHz (HFINTOSC @ 64 MHz typical)
// - Desired PWM frequency ? 62.5 kHz  (so PR reg is 0..1023)
//   PWM frequency = 64 000 000 / (1023 + 1) = 62,500 Hz
// - Left-aligned mode
// - PWM1OUT1 ? RB0  (IN1 of TB67H451)
// - PWM1OUT2 ? RB1  (IN2 of TB67H451)
//
// TB67H451 truth table:
//   IN1  IN2  Mode
//   L    L    Stop / Coast (Idle) ? standby after ~1 ms
//   H    L    Forward
//   L    H    Reverse
//   H    H    motor_brake (short)
//
// PWM control:
//   motor_forward(Pwm10Bit) : PWM on IN1, IN2 = 0
//   motor_reverse(Pwm10Bit) : IN1 = 0, PWM on IN2
// ============================================================

void load_pwm_regs (void)  {
    PWM1CONbits.LD = 1;          // Right Motor
    PWM2CONbits.LD = 1;          // Left Motor
    
#if WAIT_FOR_PWM_LOAD_DONE 
    while (PWM1CONbits.LD);    
    while (PWM2CONbits.LD);
#endif
}


// ------------------------------------------------------------
// motor_brake ? both IN1 and IN2 driven high (full duty)
// ------------------------------------------------------------
void motor_brake(void)
{
    LeftMotorPwm  = MOTOR_PWM_MAX_10BIT;   // PWM_PERIOD;     260821C
    RightMotorPwm = MOTOR_PWM_MAX_10BIT;  //  PWM_PERIOD;    
    
    PWM1S1P1 = MOTOR_PWM_MAX_10BIT;  //  PWM_PERIOD; 
    PWM1S1P2 = MOTOR_PWM_MAX_10BIT;  //  PWM_PERIOD; 

    PWM2S1P1 = MOTOR_PWM_MAX_10BIT;  //  PWM_PERIOD; 
    PWM2S1P2 = MOTOR_PWM_MAX_10BIT;  //  PWM_PERIOD;  
    
    load_pwm_regs();
    
    MovingDirection = 0;  // not moving         
}

// ------------------------------------------------------------
// motor_idle / Coast / Stop ? both outputs low
// ------------------------------------------------------------
void motor_idle(void)
{
    LeftMotorPwm  = 0;
    RightMotorPwm = 0;
    
    PWM1S1P1 = 0;
    PWM1S1P2 = 0;

    PWM2S1P1 = 0;
    PWM2S1P2 = 0;    
    
    load_pwm_regs();    
    
    MovingDirection = 0;  // not moving      
}

// ----------------------------------------------------------

void compute_right_pwm (void )  {    // 260912
    
    if (RightPwmOffset < 0)  {  // negative offset
        if (LeftMotorPwm  > (uint16_t) -RightPwmOffset)
            RightMotorPwm = LeftMotorPwm + (uint16_t) RightPwmOffset;   // Note:  '+' is correct for a negative Offset
        else
            RightMotorPwm = 0;
    }   // positive offset
    else {  // RightPwmOffset is Zero or Positive
        RightMotorPwm = LeftMotorPwm + (uint16_t) RightPwmOffset;       // won't overflow 
        if (RightMotorPwm > MOTOR_PWM_MAX_10BIT) // PWM_PERIOD)   260821C
            RightMotorPwm = MOTOR_PWM_MAX_10BIT; // PWM_PERIOD;
    }      
}

// ------------------------------------------------------------
void motor_forward (uint16_t Pwm10Bit)     // Pwm10Bit = 0 to 1023 (3FF)    PWM on IN1, IN2 held low
{
    MovingDirection = 1;  // Forward  

    LeftMotorPwm = Pwm10Bit;
    PWM2S1P1  = LeftMotorPwm;         // 260909 16 bit assignment works too
//  PWM2S1P1L = (uint8_t) LeftMotorPwm;                 
//  PWM2S1P1H = (uint8_t)(LeftMotorPwm >> 8) & 0x03;     
    PWM2S1P2  = 0;

    compute_right_pwm();    
/*
    if (RightPwmOffset < 0)  {  // negative offset
    if (RightMotorPwm > (uint16_t) -RightPwmOffset)
        RightMotorPwm = Pwm10Bit + (uint16_t) RightPwmOffset;
    else
        RightMotorPwm = 0;
    }   // positive offset
    else {  // RightPwmOffset is Zero or Positive
        RightMotorPwm = Pwm10Bit + (uint16_t) RightPwmOffset;        
        if (RightMotorPwm > MOTOR_PWM_MAX_10BIT) // PWM_PERIOD)   260821C
            RightMotorPwm = MOTOR_PWM_MAX_10BIT; // PWM_PERIOD;
    }    
*/
            
    PWM1S1P1 = RightMotorPwm;         // 260912 16 bit assignment works
//  PWM1S1P1L = (uint8_t) RightMotorPwm;                // 260821B
//  PWM1S1P1H = (uint8_t)(RightMotorPwm >> 8) & 0x03;   
    PWM1S1P2 = 0;
    
    load_pwm_regs();
        
    LastLeftMotorPwm  = LeftMotorPwm;   //  260909
    LastRightMotorPwm = RightMotorPwm;  //  260909  
}


// ------------------------------------------------------------
void motor_reverse (uint16_t Pwm10Bit)  // Pwm10Bit = 0 to 1023 (3FF)   IN1 held low, PWM on IN2
{
    MovingDirection = -1;  // Reverse     
    
    LeftMotorPwm  = Pwm10Bit;
    PWM2S1P2 = LeftMotorPwm;    
    PWM2S1P1 = 0;    

    compute_right_pwm();    
/*    
    if (RightPwmOffset < 0)  {  // negative offset
    if (RightMotorPwm > (uint16_t) -RightPwmOffset)
        RightMotorPwm = Pwm10Bit + (uint16_t) RightPwmOffset;
    else
        RightMotorPwm = 0;
    }   // positive offset
    else {  // RightPwmOffset is Zero or Positive
        RightMotorPwm = Pwm10Bit + (uint16_t) RightPwmOffset;        
        if (RightMotorPwm > MOTOR_PWM_MAX_10BIT)  // PWM_PERIOD)   260821C
            RightMotorPwm = MOTOR_PWM_MAX_10BIT;  // PWM_PERIOD;
    } 
*/ 
    PWM1S1P2 = RightMotorPwm;
    PWM1S1P1 = 0;

    load_pwm_regs();
    
    LastLeftMotorPwm  = LeftMotorPwm;   //  260909
    LastRightMotorPwm = RightMotorPwm;  //  260909     
}

// ----------------------------

void evaluate_motor_pwm (void)   {    // 260909
    
// REM:   Mean ADC value is 12 bit.
//        PWM registers are 10 bit.    
    
    
uint16_t NewMotorPwm;
uint16_t DeltaPwm;

    // Motor Pwm = PotValue / 2 + 200H     To use most of the pot range.  Pwm 0 to 200H give no motor turns.

    NewMotorPwm =  PotMeanRay [3];   // Pot3 is the control
    NewMotorPwm =  (NewMotorPwm >> 3)           + 0x200;   // apply equation above to convert to pwm value, Y = mX + B    12 bit to 10 bit, then half   260909F
//  NewMotorPwm =  (NewMotorPwm >> 1)           + 0x200;   // apply equation above to convert to pwm value, Y = mX + B
//  NewMotorPwm = ((NewMotorPwm >> 1) & 0x01FF) + 0x200;   // apply equation above to convert to pwm value, Y = mX + B
    
    // compute abs(Pwm difference)
    if (NewMotorPwm > LeftMotorPwm)
        DeltaPwm = (NewMotorPwm - LeftMotorPwm);
    else
        DeltaPwm = (LeftMotorPwm - NewMotorPwm);        
 
    if (DeltaPwm > POT_DELTA_ACTION)  {
        if (MovingDirection < 0)
            motor_reverse (NewMotorPwm);
        else
            motor_forward (NewMotorPwm);        
    }
}

// =====================================  End Grok Code

// Serial Comm Functions

char to_upper (char Key)   {           // only changes lower case chars to upper case 

    if (( Key >=  'a' )  && ( Key <=  'z' ))          // check low AND high limits  230519A
          Key -= ('a' - 'A'); 
    
    return Key;
}

// ---------- Serial Transmit Functions --------------------------------------

#define putch(txData)   UART1_Write((uint8_t) txData)


void send_CRLF (void)
   {
   putch ((char) 0x0D );  // CR
   putch ((char) 0x0A );  // LF
   }

const uint8_t NibbleToASCIIRay [16] = 
    { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

void buffer_tx_hex4 (uint8_t Data)
    {
    putch (NibbleToASCIIRay [ Data       & 0x0F]);
    }

void buffer_tx_hex8 (uint8_t Data)
    {
    putch (NibbleToASCIIRay [(Data >> 4) & 0x0F]);
    putch (NibbleToASCIIRay [ Data       & 0x0F]);
    }

void buffer_tx_dec8 (uint8_t Data)  // 240525 now correctly supports up to 255
    {
    uint8_t ShowZeros = 0; 
    uint8_t Digit;
    
    // 240525  rewrote to be correct !!
    Digit = (Data / 100);
    if (Digit != 0)  {
        putch (NibbleToASCIIRay [(Digit) & 0x0F ]);   // hundreds, NON-ZERO
        ShowZeros = 1;
        Data -= 100  * Digit;          
    }
        
    Digit = (Data / 10);                              
    if ((Digit != 0) || ShowZeros)  {
        putch (NibbleToASCIIRay [(Digit) & 0x0F ]);   // tens   could be ZERO
//      ShowZeros = 1;
        Data -= 10  * Digit;   // 240525        
    }    
    
    putch (NibbleToASCIIRay [(Data) & 0x0F ]);      // ones   
    }

// ------------------------------------------  260211

void buffer_tx_int_dec8 (int8_t Data)    {     // 260211
    if (Data < 0)  {
        putch ('-');
        buffer_tx_dec8( ~((uint8_t) Data) + 1);  // two's compliment
    }
    else
        buffer_tx_dec8((uint8_t) Data);
}

// ------------------------------------------  230924D

void buffer_tx_dec16 (uint16_t Data)   { // 0 to 64K now   

    uint16_t Digit;           
    uint8_t  ShowZeros = 0;

    Digit = (Data/10000);
    Data  = Data - (Digit * 10000);    
    if (Digit != 0)  {
        putch (NibbleToASCIIRay [(Digit) & 0x0F ]);
        ShowZeros = 1;
    }

    Digit = (Data/1000);
    Data  = Data - (Digit * 1000);        
    if ((Digit != 0) || ShowZeros) {         
        putch (NibbleToASCIIRay [(Digit) & 0x0F ]);
        ShowZeros = 1;
    }
    
    Digit = (Data/100);
    Data  = Data - (Digit * 100);    
    if ((Digit != 0) || ShowZeros) {
        putch (NibbleToASCIIRay [(Digit) & 0x0F ]);
        ShowZeros = 1;
    }

    Digit = (Data/10);
    Data  = Data - (Digit * 10);    
    if ((Digit != 0) || ShowZeros) {
        putch (NibbleToASCIIRay [(Digit) & 0x0F ]);
//      ShowZeros = 1;       not needed at this point
    }    
    
    putch (NibbleToASCIIRay [((uint8_t) Data) & 0x0F ]);  
}

// ------------------------------------

void buffer_tx_hex12 (uint16_t Data)
    {
//  putch (NibbleToASCIIRay [(Data >> 12) & 0x0F ]);
    putch (NibbleToASCIIRay [(Data >>  8) & 0x0F ]);
    putch (NibbleToASCIIRay [(Data >>  4) & 0x0F ]);
    putch (NibbleToASCIIRay [ Data        & 0x0F ]);    
    }

void buffer_tx_hex16 (uint16_t Data)
    {
    putch (NibbleToASCIIRay [(Data >> 12) & 0x0F ]);
    putch (NibbleToASCIIRay [(Data >>  8) & 0x0F ]);
    putch (NibbleToASCIIRay [(Data >>  4) & 0x0F ]);
    putch (NibbleToASCIIRay [ Data        & 0x0F ]);    
    }


// ---------------------------------------

void report_string (char * StringPtr)
   {
    char StrChar;

    while ((StrChar = *StringPtr++) != 0)   
        putch (StrChar);  
   }
  
// -------------------------------

void report_samples_and_calcs (void)  {   // of the last 8 good samples
    // Assumes the DataRay is complete and calcs have been performed
    // [0]=mean  [1..8] = samples  [9]=diffsum  
            
    uint8_t i;
    for (i=0; i<10; i++ )  {
        buffer_tx_hex16 (DataRay[i]);
        putch (' ');
        if (i == 4)
            putch (' '); 
    }
    send_CRLF();
}

// -------------------------------

void report_pots ()  {
    
    uint8_t i;    
    
//  if (!UART1_IsTxReady()) return;
    
    for (i=1; i<4; i++)  {
        
#if (POT_REPORT == POT_MEANS_REPORT)
        buffer_tx_hex16 (PotMeanRay[i]);        // SUM OF 8 samples, X 2 for easy comparison to DataRay, shifted by 4 bits

#elif (POT_REPORT == POT_SUM8_REPORT)
        buffer_tx_hex16 (PotSum8Ray[i]);        // SUM OF 8 samples
        
#elif (POT_REPORT == POT_SUM8_TIMES2_REPORT)
        buffer_tx_hex16 (PotSum8Ray[i] << 1);   // SUM OF 8 samples, X 2 for easy comparison to DataRay, shifted by 4 bits
        
#endif
//      buffer_tx_hex16 (PotSum8Ray[i] << 1);        // SUM OF 8 samples, X 2 for easy comparison to DataRay, shifted by 4 bits
        putch (' ');

        // Report Diffsum as just two digits, since usually low number
        if (PotDiffSumRay[i] > 0xFF)
            buffer_tx_hex8 ((uint8_t) 0xFF);
        else
            buffer_tx_hex8 ((uint8_t) PotDiffSumRay[i]);
        putch (' ');
        putch (' ');
    }
    send_CRLF();    
}

// -------------------------------------

void report_ir_sensor_byte (void)   {   // 1's are positive reflections from white paper
    
    buffer_tx_hex8 ((uint8_t) IR_SENSORS);     // 260911   Use Macro with inversions    
//  buffer_tx_hex8 ((uint8_t) PORTC & 0xFE);   // 260911   Only Bits 7..1 used for Sensors    
    
}

// -------------------------------------

void report_motors (void)  {
    
//  if (!UART1_IsTxReady()) return;

    report_string (" Lpwm=");
    buffer_tx_hex12 (LeftMotorPwm);
    report_string ("  RPwm=");
    buffer_tx_hex12 (RightMotorPwm);
    report_string ("  Sensors=");
    report_ir_sensor_byte ();
    send_CRLF();
}

// -------------------------------------
            
void report_verbose_mode_labels(void)  {         // 260909G
    
    send_CRLF();
    if (VerboseMode == VERBOSE_POT_VALUES)  { // 1
        report_string ("Report POT Values\n");
        report_string ("POT1 Er  POT2 Er  POT3 Er\n");
    }
    else 
    if (  VerboseMode == VERBOSE_MOTOR_PWM_VALUES)  // 2
        report_string ("Report Motor Pwm Values\n");  
    else
    if ( VerboseMode == VERBOSE_DATARAY_DUMP) {  // 3
        report_string ("Report POT ");
        putch (ReportingDataRayForPotNum + 0x30);
        putch (' ');
        report_string ("DataRay[]\n");
        report_string ("Mean [1]  [2]  [3]  [4]   [5]  [6]  [7]  [8]  DiffSum\n");          
    }
    else   {     
        VerboseMode = 0;  
        report_string ("Quiet, no reports now.\n");                    
    }    
}     
            
// ------------------------------------------------

void power_opto_ir_leds (uint8_t OptoByte)   {   // see shifting specs for the 74HC595

    uint8_t  i;

    RCLK_LOW;
    SCLRN_HIGH;

    for (i=0; i<8; i++)  {
        SCLK_LOW;          
        if (OptoByte & 0x80)  // test MSBit always
            SERIN_HIGH;
        else
            SERIN_LOW;
        OptoByte = (uint8_t) (OptoByte << 1);  // and buy some time for Serial Data to settle   260820
        SCLK_HIGH;                 // clocks SERIN into LSBit of 8 bit serial chain
    }

    RCLK_HIGH;   // Applies serial shift register to output register
//  RCLK_LOW;   don't here - do at the beginning of the function
}

// ------------------------------------------------

void report_led_mode_and_init (void)   {   // 260910
    
    if (LedMode == LED_FOLLOWS_SENSOR4)  {
        power_opto_ir_leds(0x10);   // turn on LED 4 (bit4) only
        report_string ("\nLED2 follows middle sensor4.  LED2 is ON with reflection, OFF without.\n");   
    }
    else
        power_opto_ir_leds(0x00);   // turn all sensors OFF 

    if (LedMode == LED_TOGGLES_AT_2HZ)
        report_string ("\nToggling LED at 2Hz.\n");                

    if (LedMode == LED_TOGGLES_WITH_SWITCH)  {     // Good for debug - turn LED_ON anywhere in your code
        report_string ("\nLED is Toggled On/Off with the Switch.\n"); 
        LED_OFF;   // start here
    }    
}

// ------------------------------------------------

uint8_t  LastUserChar = ' ';        // 260219


void process_key (uint8_t RxChar) {

    RxChar = (uint8_t) to_upper((char) RxChar);
    
    if (RxChar == '=')   // change it
        RxChar = '+';
    
    // 251126
    putch (RxChar);    
    if (RxChar == 0x0D)  { //  if CR char
        putch(0x0A);     //  send LF too 
        return;
    }


    switch (RxChar)  {
        
        case '?':
            report_string (" (L)ed  (P)otMotor  (D)rive (R)everse (+)faster (-)slower (I)dle (B)rake (V)erbose\n");
            report_string (" Optos: (A)ll  (C)enter  (W)ider  (N)arrower\n");   // 260911
            break;        

        case    'A':    // 260911
            report_string (" All Opto IR LEDs Powered.\n");
            OptoLEDs = 0xFE;
            power_opto_ir_leds (OptoLEDs);    // 260911                 
            break;            
            
        case    'C':    // 260911
            report_string (" Center 3 Opto IR LEDs Powered.\n");
            OptoLEDs = 0x38;   // center 3
//          OptoLEDs = 0x10;   // just ONE            
            power_opto_ir_leds (OptoLEDs);    // 260911                 
            break;                   
            
        case    'W':    // 260911
            report_string (" Wider Beam Opto IR LED= 0x");
            if (OptoLEDs & 0x40)
                OptoLEDs = 0xFE;    // widest beam, all IR LEDs on
            else 
            if (OptoLEDs & 0x20)
                OptoLEDs = 0x7C;
            else 
                OptoLEDs = 0x38;
            buffer_tx_hex8 (OptoLEDs);
            send_CRLF();
            power_opto_ir_leds (OptoLEDs);    // 260911                 
            break;                
            
        case    'N':    // 260911
            report_string (" Narrower Beam Opto IR LED= 0x");
            if (OptoLEDs & 0x80)  // widest beam, assume all are on
                OptoLEDs = 0x7C;
            else 
            if (OptoLEDs & 0x40)  // narrower beam, assume 7C, all but outside IR LEDs on 
                OptoLEDs = 0x38;
            else
            if (OptoLEDs & 0x20)  // narrower beam, assume 38, just center 3 IR LEDs on 
                OptoLEDs = 0x10;                       
            buffer_tx_hex8 (OptoLEDs);
            send_CRLF();
            power_opto_ir_leds (OptoLEDs);    // 260911            
            break;                  
            
            
        case    '1':
            ReportingDataRayForPotNum = 1;
            VerboseMode = VERBOSE_DATARAY_DUMP;
            report_verbose_mode_labels();    
            break;
            
        case    '2':
            ReportingDataRayForPotNum = 2;
            VerboseMode = VERBOSE_DATARAY_DUMP;   
            report_verbose_mode_labels();            
            break;

        case    '3':
            ReportingDataRayForPotNum = 3;
            VerboseMode = VERBOSE_DATARAY_DUMP; 
            report_verbose_mode_labels();             
            break;            

        case 'P':    // Pots control the motors TOGGLE to Pots vs Serial control
            if (PotsControlMotors) {
                PotsControlMotors = 0;
                report_string (" Use the (+) and (-) keys to change speed.\n");
            }
            else  {
                PotsControlMotors = 1;
                report_string (" Pots control motors.\n");

                if (MovingDirection > 0)
                    motor_forward (LeftMotorPwm);
                else if (MovingDirection < 0)
                    motor_reverse (LeftMotorPwm);

                LastLeftMotorPwm = LeftMotorPwm;   // really for the case where MovingDirection == 0                 
            }
            break;             

            
        case 'V':    // Verbose modes
            if (++VerboseMode > 3)
                VerboseMode = 0;

            if (VerboseMode == VERBOSE_DATARAY_DUMP) 
                ReportingDataRayForPotNum = 3;   // POT #3, for motor speed
            
            report_verbose_mode_labels();
            break;         

        case 'I':    // Idle
            PotsControlMotors = 0;  // 260911B            
            motor_idle();            
            break; 

        case 'B':    // motor_brake
            PotsControlMotors = 0;  // 260911B
            motor_brake();
            break; 
            
        case 'D':    // Drive forward motor action
            PotsControlMotors = 0;  // 260911B   
            motor_forward (LeftMotorPwm);
            break;   
       

        case 'R':    // Reverse motor action
            PotsControlMotors = 0;  // 260911B            
            motor_reverse (LeftMotorPwm);           
            break;   

        case '+':    // Go Faster 
        case '=':
            PotsControlMotors = 0;  // 260911B                  
            LeftMotorPwm += MOTOR_PWM_USER_DELTA;
            if (LeftMotorPwm > MOTOR_PWM_MAX_10BIT)
                LeftMotorPwm = MOTOR_PWM_MAX_10BIT;
            if (MovingDirection < 0)
                motor_reverse (LeftMotorPwm);
            else
                motor_forward (LeftMotorPwm);
            break;  

        case '-':    // Go Slower
        case '_':
            PotsControlMotors = 0;  // 260911B                  
            if (MOTOR_PWM_USER_DELTA <= LeftMotorPwm)
                LeftMotorPwm -= MOTOR_PWM_USER_DELTA;
            else
                LeftMotorPwm = 0;
            if (MovingDirection < 0)
                motor_reverse (LeftMotorPwm);
            else
                motor_forward (LeftMotorPwm);          
            break;              

        case 'X':  
            break;
    
        case ' ':    // Idle 
            PotsControlMotors = 0;  // 260911B                  
            motor_idle();
            VerboseMode  = 0;              
            break;        
                    
        case 'L':    // Toggle LED Mode      260910 sequenced, not compiler directive
            LedMode += 1;
            if (LedMode >= NUM_OF_LED_MODES)
                LedMode = 0;
            report_led_mode_and_init ();
            break;     
            
        case 0x0A:   // LF   251115
            send_CRLF();
            break;
            
        default:
            return;
            break;
    }
}

// ------------------------------------------------

volatile uint8_t Timer0Ticks     = 0;    // 260912 added volatile
volatile uint8_t Timer100mSTicks = 0;

// ------------------------------------------------------------------------------------------------------

#define ADC_STOP_CONVERSION       ADCON0bits.ADGO = 0   // ADC_BIT_CLEAR
#define ADC_START_CONVERSION      ADCON0bits.ADGO = 1   // ADC_BIT_SET
//                                ADCON0bits.ADGO = ADC_BIT_SET;

#define ADC_CONVERT_DONE        (!ADCON0bits.ADGO)      // 260323
#define ADC_STILL_CONVERTING      ADCON0bits.ADGO       // 260323

// --------------------------------

uint16_t  SumOf8;
uint16_t  DiffSumOf8;
uint16_t  MeanOf8;
int16_t   iDiff16;

uint8_t DoHalfSecondReports = 0;


void compute_sample_means (uint8_t Chan)  {
    
    uint8_t i;

    SumOf8 = 0;
    for (i=1; i<9; i++)
        SumOf8 += DataRay [i];
    
    LastPotSum8Ray[Chan] = PotSum8Ray [Chan]; 
    PotSum8Ray   [Chan] = SumOf8;
    
    MeanOf8 = (SumOf8 >> 3);   // divide by 8 for comparison to each ADC value
    DataRay[0] = MeanOf8;      // save here - not really used though    
    PotMeanRay[Chan] = MeanOf8;
    
    DiffSumOf8  = 0;
    for (i=1; i<9; i++)  {
        iDiff16 = (int16_t) DataRay [i] - (int16_t) MeanOf8;
        if (iDiff16 < 0)
            iDiff16 = -iDiff16;
        DiffSumOf8 += (uint16_t) iDiff16;
    }   
    DataRay[9] = DiffSumOf8;   // save here - not really used though      
    PotDiffSumRay[Chan] = DiffSumOf8;     
}

// ===============================================

// void __interrupt(irq(IRQ_TMR0), high_priority) MyTimer0_ISR(void)  {   //   doesn't work since vector table already populated
 
void MyTimer0_ISR (void)   {      // HIGH PRIORITY INT
    
    // 250Hz for system timebase  >> HIGH PRIORITY <<
    
    if (!PendingPotToCompute  && !AdcHoldoffCount)  
    {
        if (MuxIndex > 2) {     // then it was grounded, used between pot changes

            ADPCH    = NextMuxIndex;    //  assume settles in about 5uS
            MuxIndex = NextMuxIndex;
            if (++NextMuxIndex > 2)    // ADC MUX 0, 1, 2,  0x3B(gnd)
                NextMuxIndex   = 0;            
                 
            DataRayIndex = 0;    // for SampleRay, throw away first one at  DataRayIndex[0] next interrupt
            ADC_START_CONVERSION;
            AdcHoldoffCount = ADC_SAMPLES_TO_TOSS;    // toss these next N samples for RC equilib time due to 10K impedance
        }
        else if (ADC_CONVERT_DONE)
        {
            DataRay[DataRayIndex] = (uint16_t) ADC_ConversionResultGet(); 
            if (++DataRayIndex > 8)  {
                DataRayIndex   = 0;
                PendingPotToCompute = MuxIndex+1;  // non zero  AdcCh0,1,2 --> DataRay Pot1,2,3

                SET_ADC_MUX_TO_GND;    //  assigns ADPCH and MuxIndex to 0x3B  - assume settles in about 5uS                
//              LED_ON;
            }
            else
                ADC_START_CONVERSION;
        }
    }

    if (AdcHoldoffCount)             // 260908 Count Down
        AdcHoldoffCount -= 1; 
    
    
    if (++Timer0Ticks >= 25)  {
        Timer0Ticks   -= 25;
        Timer100mSTicks += 1;        
    }    

/*  260912  Grok says this is done for us in the RETFIE
   
#if USE_TWO_PRIORITY_LEVEL_INTERRUPTS   
    INTCON0bits.GIEH = 1;         // 260907      
    INTCON0bits.GIEL = 1;         // ? ADD THIS LINE    GROK    260323
#else
    INTCON0bits.GIE = 1;
#endif        
*/    
}

// -----------------------------------------------------------------

void check_time_actions (void)  {

    if (Timer100mSTicks >= 5)  {    // 1/2 second things
        
        // --------------------------
        // First, safely Reduce Timer100mSTicks
        
#if USE_TWO_PRIORITY_LEVEL_INTERRUPTS   // 260907B 
    INTCON0bits.GIEH = 0;          
#else
    INTCON0bits.GIE = 0;    
#endif
        
        Timer100mSTicks -= 5;   // atomic operation, do not interrupt midstream
        
#if USE_TWO_PRIORITY_LEVEL_INTERRUPTS   // 260907B
    INTCON0bits.GIEH = 1;          
#else
    INTCON0bits.GIE = 1;    
#endif
        // --------------------------
        
        DoHalfSecondReports = 1;

        if (LedMode == LED_TOGGLES_AT_2HZ)  {   // 260910
            if (!IS_SWITCH_PRESSED)  {
                if (IS_LED_ON)
                    LED_OFF;
                else
                    LED_ON;
            }  
        }      
    }
}


// -----------------------------------------------------------------

int main(void)
{
    SYSTEM_Initialize();

    // Explicitly set all input pins to Schmitt Trigger (recommended)
    INLVLA = 0xFF;
    INLVLB = 0xFF;
    INLVLC = 0xFF;

    // High Priority Interrupts  -----------------------------

    TMR0_PeriodMatchCallbackRegister (MyTimer0_ISR);    //   4mS, so 250Hz   260912
//  TMR0_OverflowCallbackRegister    (MyTimer0_ISR);    //   4mS, so 250Hz    depreciated
    
    
    // Low Priority Interrupts  -----------------------------

//  TMR1_OverflowCallbackRegister   (MyTimer1_ISR);    //  Motor operation interrupt every 30mS    
    
    // Turn ON interrupts    
#if USE_TWO_PRIORITY_LEVEL_INTERRUPTS    // 260820
    INTCON0bits.GIEH = 1; 
    INTCON0bits.GIEL = 1;
#else
    INTCON0bits.GIE = 1;
#endif        
    
    
    // Make VREF for Pots and for Motor current threshold
    DAC1_SetOutput(0xFF);   // max value, around 4.096 or 4.080V measured       
    
    // Enable the PWM channels
    PWM1CONbits.EN = 1;      // Loads registers too
    PWM2CONbits.EN = 1;      // Loads registers too
    
    motor_forward (0x0000);   // start moving for debug
//  motor_forward (0x0100);   // start moving for debug    
    
    report_string (HELLO_STRING);   // Firmware number
    send_CRLF();      

    power_opto_ir_leds (OptoLEDs);    // 260911
//  power_opto_ir_leds(0x38);   // turn on LEDs 5,4,3 only    
//  power_opto_ir_leds(0xFE);   // turn all the sensor LEDs, Sensor 7..1  LSB not used.
    report_string ("\nIR LEDs Powered= 0x");
    buffer_tx_hex8 (OptoLEDs);
    send_CRLF();

    report_led_mode_and_init ();    
    
    report_verbose_mode_labels();    // 260909G
    
    DataRayIndex = 0;        // for SampleRay, throw away first one at  DataRayIndex[0] next interrupt
    SET_ADC_MUX_TO_GND;      //  assume settles in about 5uS
    ADC_START_CONVERSION;      

    // ------------------------------------------------
    
    while(1)
    {
        check_time_actions ();   // 260820
        
        if (PendingPotToCompute)  {   // acquired 8 good ADC samples after ignoring the 0'th one
            
            compute_sample_means (PendingPotToCompute);  // 1..3
            
            if ( DoHalfSecondReports )
            if ( VerboseMode    == VERBOSE_DATARAY_DUMP    )   // 3 
            if ( PendingPotToCompute == ReportingDataRayForPotNum)  {
                report_samples_and_calcs(); 
                DoHalfSecondReports = 0;
            }
            
            if (PendingPotToCompute == 3)    // 260909
            if (PotsControlMotors)                
                evaluate_motor_pwm ();   // and update pwm if pot has changed much       

            PendingPotToCompute = 0;   // ONLY RESET here
        }   

        if (DoHalfSecondReports)  {
            
            if (VerboseMode == VERBOSE_POT_VALUES)    {  // 1
                report_pots(); 
                DoHalfSecondReports = 0;
            }
            else 
            if (VerboseMode == VERBOSE_MOTOR_PWM_VALUES)   {  // 2
                report_motors();            
                DoHalfSecondReports = 0;                
            }
        }
        
        if (UART1_IsRxReady())   {  // RX char has been received
            
            RxChar = UART1_Read();
            process_key (RxChar);
        }         

        if (LedMode == LED_FOLLOWS_SENSOR4)  {  // 260910
            if (IS_SENSOR4_HIGH)   // then no received light from reflection (BLACK)
                LED_OFF;
            else                   // received reflected light, WHITE
                LED_ON;
        }
        
        if (LedMode == LED_TOGGLES_WITH_SWITCH)  {  // 260910  -- Note:  NOT Debounced
            if (IS_SWITCH_PRESSED)  {  // reset the LED to OFF
                if (IS_LED_ON)         // just do this once
                    LED_OFF;
                else
                    LED_ON;
            }
        }       
    }    
}