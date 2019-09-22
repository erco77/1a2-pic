// vim: autoindent tabstop=8 shiftwidth=4 expandtab softtabstop=4

/*
 * File:   main.c
 * Author: Greg Ercolano, erco@seriss.com
 *
 * Created on Apr 24, 2019, 06:29 PM
 * Compiler: MPLAB X IDE V5.10 + XC8 -- Microchip.com
 *
 * Drive the 1A2 Multiline Phone Control board, CPU2 Firmware.
 *                               _    _
 *                           V+ | |__| | GND
 *              x (OUT) -- RA5  |      | RA0 -- (OUT) EXT8 BUZZ
 *              x (OUT) -- RA4  |      | RA1 -- (OUT) EXT7 BUZZ
 *        (MCLR) X (IN) -- RA3  |      | RA2 -- (OUT) EXT6 BUZZ
 *      MT8870 STD (IN) -- RC5  |      | RC0 -- (OUT) EXT5 BUZZ
 *     CPU STATUS (OUT) -- RC4  |      | RC1 -- (IN) ROTARY PULSE
 *      EXT4 BUZZ (OUT) -- RC3  |      | RC2 -- (IN) DTMF 'd'
 *      EXT3 BUZZ (OUT) -- RC6  |      | RB4 -- (IN) DTMF 'c'
 *      EXT2 BUZZ (OUT) -- RC7  |      | RB5 -- (IN) DTMF 'b'
 *      EXT1 BUZZ (OUT) -- RB7  |______| RB6 -- (IN) DTMF 'a'
 *
 *                         PIC16F1709 / CPU2
 *                           REV G, G1, H
 *
 */

// REVISION G / CPU2                                    Port(ABC)
//                                   76543210           |Bit# in port
// Inputs                            ||||||||           ||
#define MT8870_STD     ((G_portc & 0b00100000)?1:0) // RC5: goes hi when dial button pressed
#define DTMF_a         ((G_portb & 0b01000000)?1:0) // RB6: data bit 'a' from MT8870 of which dial button pressed
#define DTMF_b         ((G_portb & 0b00100000)?1:0) // RB5: data bit 'b' from MT8870 of which dial button pressed
#define DTMF_c         ((G_portb & 0b00010000)?1:0) // RB4: data bit 'c' from MT8870 of which dial button pressed
#define DTMF_d         ((G_portc & 0b00000100)?1:0) // RC2: data bit 'd' from MT8870 of which dial button pressed
#define ROTARY_PULSE   ((G_portc & 0b00000010)?1:0) // RC1: rotary pulse
#define SECONDARY_DET  ((G_porta & 0b00100000)?1:0) // RA5: detects if card configured as SECONDARY (JP4) [currently unused]

// Outputs
#define CPU_STATUS_LED LATCbits.LATC4               // RC4: hi to turn LED on
                                                                                  __
#define EXT1_BUZZ      LATBbits.LATB7               // hi to turn EXT1 buzzer on    |
#define EXT2_BUZZ      LATCbits.LATC7               // hi to turn EXT2 buzzer on    |__ EXT 1-4
#define EXT3_BUZZ      LATCbits.LATC6               // hi to turn EXT3 buzzer on    |   PRIMARY
#define EXT4_BUZZ      LATCbits.LATC3               // hi to turn EXT4 buzzer on  __|
                                                    //                            __
#define EXT5_BUZZ      LATCbits.LATC0               // hi to turn EXT5 buzzer on    |
#define EXT6_BUZZ      LATAbits.LATA2               // hi to turn EXT6 buzzer on    |__ EXT 5-8
#define EXT7_BUZZ      LATAbits.LATA1               // hi to turn EXT7 buzzer on    |   SECONDARY
#define EXT8_BUZZ      LATAbits.LATA0               // hi to turn EXT8 buzzer on  __|

// This must be #defined before #includes
#define _XTAL_FREQ 8000000UL        // system oscillator speed in HZ (__delay_ms() needs this)

// --- The following section copy/pasted from MPLAB X menu: Production -> Set Configuration Bits -> Generate Source..
// CONFIG1
#pragma config FOSC     = INTOSC    // USE INTERNAL OSCILLATOR: Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE     = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE    = OFF       // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE    = ON        // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP       = OFF       // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN    = OFF       // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF       // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO     = OFF       // Internal/External Switchover Mode (Internal/External Switchover Mode is disabled)
#pragma config FCMEN    = OFF       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT     = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PPS1WAY = ON         // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software)
#pragma config ZCDDIS  = ON         // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR)
#pragma config PLLEN   = OFF        // Phase Lock Loop enable (4x PLL is enabled when software sets the SPLLEN bit)
#pragma config STVREN  = ON         // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV    = LO         // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR   = OFF        // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP     = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)
// --- end section

// PIC hardware includes
#include <xc.h>                     // our Microchip C compiler (XC8)
#include "Debounce.h"               // our signal debouncer module

// DEFINES
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long
#define ITERS_PER_SEC        500    // while() loop iters per second (Hz). *MUST BE EVENLY DIVISIBLE INTO 1000*
#define ROTARY_BUZZ_MSECS    800    // how many msecs a rotary dialed extension's buzzer should buzz (almost 1 sec)
#define ROTARY_MAX_OFF_MSECS 200    // determines when dialing completed
#define ROTARY_POWERUP_MSECS 500    // how long to wait after powerup before doing rotary detect

// Rotary dialing struct
//     Variables related to rotary dial management
//
typedef struct {
    uchar mode;        // 0=idle, 1=digit pulse, 2=digit space, 3=dialing completed
    uchar digit;       // rotary digit dialed
    int   on_msecs;    // #msecs debounced rotary detector was "on"
    int   off_msecs;   // #msecs debounced rotary detector was "off"
    long  buzz_msecs;  // counts how long to run buzzer after rotary dialing an extension
} Rotary;

// GLOBALS
const long  G_msecs_per_iter = (1000/ITERS_PER_SEC); // #msecs per iter (if ITERS_PER_SEC=125, this is 8)
long        G_powerup_msecs  = 0;                    // counts up from 0 to 10,000 then stops
long        G_msec           = 0;                    // counts up from 0 to 1000, steps by G_msecs_per_iter, wraps to zero.
uchar       G_porta, G_portb, G_portc;               // 8 bit input sample buffers (once per main loop iter)
volatile char G_buzz_ext     = -1;                   // extension to buzz (-1 for none, 0 for all)
                                                     // (used by isr() to run buzzer)
// Initialize debounce struct for rotary input values
//
//     'value' range:
//         max_value     20         __________________
//                                 /
//         on thresh     15 ....../...................
//                               /
//         off thresh    8 ...../.....................
//                             /
//                       0 ___/
//
void RotaryDebounceInit(Debounce *d) {
    d->value      = 0;
    d->max_value  = 15; // 20
    d->on_thresh  = 10; // 15;
    d->off_thresh = 4;  // 8;
    d->thresh     = 10; // 15;
}

// Initialize the Rotary struct's values to zero
void RotaryInit(Rotary *r) {
    r->mode       = 0;
    r->digit      = 0;
    r->on_msecs   = 0;
    r->off_msecs  = 0;
    r->buzz_msecs = 0;
}

// Flash the CPU2 STATUS led two quick blinks per second
void FlashCpuStatusLED() {
    // 2 quick blinks per second
    //
    //             1 second (1000ms)
    //      :<---------------------------->:
    //      :_____       _____             :
    //      |     |     |     |            :
    //  ____|     |_____|     |____________:
    //
    //      :     :     :     :            :
    //      : 150 : 150 : 150 :    550     :
    //      :  ms :  ms : ms  :     ms     :
    //      :     :     :     :            :
    //      0     150   300   450          1000
    //
    CPU_STATUS_LED = (G_msec <= 150) || (G_msec >= 300 && G_msec <= 450) ? 1 : 0;
}

// See p.xx of PIC16F1709 data sheet for other values for PS (PreScaler) -erco
#define PS_256  0b111
#define PS_128  0b110
#define PS_64   0b101
#define PS_32   0b100
#define PS_16   0b011
#define PS_8    0b010
#define PS_4    0b001
#define PS_2    0b000
//                 \\\_ PS0 \    Together these are
//                  \\_ PS1  |-- the PS bits of the
//                   \_ PS2 /    OPTION_REG.

// Set the timer0 speed (timer0 prescaler value).
//     'val' must be one of the PS_### macros defined above.
//
void SetTimerSpeed(int val) {
    OPTION_REGbits.PS = val;                 // set PreScaler (PS) value hardware bits
}

// Initialize PIC chip I/O for REV E board.
void Init() {
    OPTION_REGbits.nWPUEN = 0;   // Enable WPUEN (weak pullup enable) by clearing bit

    // Set PIC chip oscillator speed
    OSCCONbits.IRCF   = 0b1110;  // 0000=31kHz LF, 0111=500kHz MF (default on reset), 1011=1MHz HF, 1101=4MHz, 1110=8MHz, 1111=16MHz HF
    OSCCONbits.SPLLEN = 0;       // disable 4xPLL (PLLEN in config words must be OFF)
    OSCCONbits.SCS    = 0b10;    // 10=int osc, 00=FOSC determines oscillator

    // In the following TRISA/B/C data direction registers,
    //       '1' configures an input, '0' configures an output.
    //       'X' indicates a don't care/not implemented on this chip hardware.
    //       NOTE: A3 is INPUT ONLY.
    TRISA  = 0b00001000; // data direction for port A (0=output, 1=input)
    WPUA   = 0b00001000; // enable 'weak pullup resistors' for all inputs
    //         ||||||||_ A0 (OUT) EXT8 BUZZ
    //         |||||||__ A1 (OUT) EXT7 BUZZ
    //         ||||||___ A2 (OUT) EXT6 BUZZ
    //         |||||____ A3 (IN)  unused/MCLR
    //         ||||_____ A4 (OUT) unused
    //         |||______ A5 (OUT) unused
    //         ||_______ X
    //         |________ X

    TRISB  = 0b01110000; // data direction for port B (0=output, 1=input)
    WPUB   = 0b01110000; // enable 'weak pullup resistors' for all inputs
    //         ||||||||_ X
    //         |||||||__ X
    //         ||||||___ X
    //         |||||____ X
    //         ||||_____ B4 (IN)  DTMF 'c'
    //         |||______ B5 (IN)  DTMF 'b'
    //         ||_______ B6 (IN)  DTMF 'a'
    //         |________ B7 (OUT) EXT1 BUZZ

    TRISC  = 0b00100110; // data direction for port C (0=output, 1=input)
    WPUC   = 0b00100110; // enable 'weak pullup resistors' for all inputs
    //         ||||||||_ C0 (OUT) EXT5 BUZZ
    //         |||||||__ C1 (IN)  ROTARY_PULSE
    //         ||||||___ C2 (IN)  DTMF 'd'
    //         |||||____ C3 (OUT) EXT4 BUZZ
    //         ||||_____ C4 (OUT) CPU STATUS
    //         |||______ C5 (IN)  MT8870 STD
    //         ||_______ C6 (OUT) EXT3 BUZZ
    //         |________ C7 (OUT) EXT2 BUZZ

    // Disable analog stuff
    ANSELA = 0x0;
    ANSELB = 0x0;
    ANSELC = 0x0;
    ADCON0 = 0x0;   // disables ADC

    // Disable slew rate controls
    SLRCONA = 0x0;
    SLRCONB = 0x0;
    SLRCONC = 0x0;

    PORTA = 0x0;
    PORTB = 0x0;
    PORTC = 0x0;

    // ENABLE TIMER0 INTERRUPT TO RUN BUZZER
    {
        INTCONbits.GIE        = 1;          // Global Interrupt Enable (GIE)
        INTCONbits.PEIE       = 1;          // PEripheral Interrupt Enable (PEIE)
        INTCONbits.TMR0IE     = 1;          // timer 0 Interrupt Enable (IE)
        INTCONbits.TMR0IF     = 0;          // timer 0 Interrupt Flag (IF)
        // Configure timer
        OPTION_REGbits.TMR0CS = 0;          // set timer 0 Clock Source (CS) to the internal instruction clock
        OPTION_REGbits.TMR0SE = 0;          // Select Edge (SE) to be rising (0=rising edge, 1=falling edge)
        OPTION_REGbits.PSA    = 0;          // PreScaler Assignment (PSA) (0=assigned to timer0, 1=not assigned to timer0)
        // Set timer0 prescaler speed
        SetTimerSpeed(PS_32);               // Sets prescaler (divisor) to run timer0
        ei();                               // enable ints last (sets up our isr() function to be called by timer interrupts)
    }
}

// Buzz the specified extension number (1 thru 8) at 60Hz
//     If extension number is 10 (dialed "0"), buzzes ALL extensions (added for Stephane Levesque 06/27/2019).
//     If extension number is -1, all extension buzzers are turned off.
//
void BuzzExtension(int num) {
    static uchar count = 0;
    uchar bz_60hz = (++count & 1);      // bz is 0|1 changing at ~60hz rate

    if ( num < 0 || num > 10 ) bz_60hz = 0;  // force bz off if ext# outside 1-8 range

    // Drive the extension buzzers
    EXT1_BUZZ = ( num == 10 || num == 1 ) ? bz_60hz : 0;
    EXT2_BUZZ = ( num == 10 || num == 2 ) ? bz_60hz : 0;
    EXT3_BUZZ = ( num == 10 || num == 3 ) ? bz_60hz : 0;
    EXT4_BUZZ = ( num == 10 || num == 4 ) ? bz_60hz : 0;
    EXT5_BUZZ = ( num == 10 || num == 5 ) ? bz_60hz : 0;
    EXT6_BUZZ = ( num == 10 || num == 6 ) ? bz_60hz : 0;
    EXT7_BUZZ = ( num == 10 || num == 7 ) ? bz_60hz : 0;
    EXT8_BUZZ = ( num == 10 || num == 8 ) ? bz_60hz : 0;
}

// Interrupt service routine
//     Handles oscillating selected buzzer at hardware controlled rate of speed
//     This runs at approx 60Hz
//
void __interrupt() isr(void) {
    static char count = 0;          // DEBUG
    if ( INTCONbits.TMR0IF ) {      // int timer overflow?
        INTCONbits.TMR0IF = 0;      // clear bit for next overflow
        BuzzExtension(G_buzz_ext);  // run selected buzzer
    }
}

// Reset the rotary counters and deenergize any buzzers
void RotaryReset(Rotary *r) {
    RotaryInit(r);
    G_buzz_ext = -1;        // all buzzer coils off
}

// HANDLE ROTARY DIALING ON INTERCOM LINE
// Debounce the potentially noisy input and include snap action hysteresis.
// Empirical tests showed noise to look like this:
//
//                               Rotary           Rotary
//                               Pulse    Space   Pulse
//                             ____^____   /\   ____^____
//                            /         \ /  \ /         \
//
//                             <--65ms--><20ms> <--65ms-->
//       ROTARY_PULSE:         _________        _________
//                         || |         |   || |         |
//                 ________||_|         |___||_|         |_______________
//
//                         \/               \/
//                       noise            noise
//
void HandleRotaryDialing(Rotary *r, Debounce *d) {
    // Debounce the "ROTARY_PULSE" input
    int is_rotary_pulse = DebounceNoisyInput(d, ROTARY_PULSE);

    // Early exit if idle & not dialing (mode=0) or valid digit still being handled (mode=3)
    if ( !is_rotary_pulse && ( r->mode == 0 || r->mode == 3 ) ) return;

    // COUNT ROTARY PULSES
    //    Manage separate counters for pulse and space
    //
    if ( is_rotary_pulse ) {
        // PULSE
        if ( r->mode == 0 || r->mode == 3 ) // Begin new dialing sequence?
             RotaryReset(r);                // Reset rotary system, stop buzzers
        r->mode = 1;
        if ( r->off_msecs )                 // Just finished counting "off time"?
            r->off_msecs = 0;               // ..then this is rising edge: zero off timer
        r->on_msecs += G_msecs_per_iter;    // keep count of "on time"

    } else {
        // SPACE BETWEEN PULSES
        r->mode = 2;
        // Just finished on time? Falling edge: zero on timer & count digit
        if ( r->on_msecs ) { r->on_msecs = 0; r->digit++; }
        r->off_msecs += G_msecs_per_iter;   // count off time
        if ( r->off_msecs > ROTARY_MAX_OFF_MSECS ) {
            // Full digit dialed -- stop dialing and start buzzer
            r->mode = 3;                    // indicate r->digit now has valid digit
            r->buzz_msecs = ROTARY_BUZZ_MSECS;
        }
    }
}

// Return rotary dialed digit, or -1 if none.
//     Manages keeping digit active until rotary buzzer timer expires.
//     Also handles preventing switch hook pickup noise from causing false dialing.
//     NOTE: When "0" is dialed, a 10 is generated
//
int GetRotaryDigit(Rotary *r, Debounce *d) {
    // Early exit if recent pickup
    if ( G_powerup_msecs < ROTARY_POWERUP_MSECS ) return -1;

    HandleRotaryDialing(r, d);                        // loads r->digit + r->mode

    // Valid digit recently dialed (mode=3), and buzzer still running?
    if ( r->mode == 3 && r->buzz_msecs > 0 ) {
        r->buzz_msecs -= G_msecs_per_iter;            // count buzz timer down to zero
        if ( r->buzz_msecs <= 0 ) RotaryReset(r);     // stop buzzer, done
        else                      return r->digit;    // return digit to keep buzzing
    }
    return -1;
}

// Return DTMF dialed digit, or 0 if none.
//     NOTE: when a "0" is dialed, the MT8870 generates "10".
//
int GetDTMFDigit() {
    // Nothing being dialed right now? early exit
    if ( !MT8870_STD ) return 0;

    // If MT8870_STD is 1, a Touch-Tone button is pressed; decode the digit
    // from the MT8870 abcd outputs as an integer, and buzz that extension
    //
    return (( DTMF_a ) | (DTMF_b << 1) | (DTMF_c << 2) | (DTMF_d << 3));
}

// Buffer the hardware state of PIC's PORTA/B/C all at once.
//     Run this at the beginning of each iter of the 125Hz main loop.
//     We then do bit tests on these buffered values, to avoid multiple
//     hardware reads throughout execution to avoid sampling parallax.
//
void SampleInputs() {
    // Buffer the hardware input states
    G_porta = PORTA;
    G_portb = PORTB;
    G_portc = PORTC;
}

void main(void) {
    int digit;          // dtmf or rotary dialed digit
    Rotary rot;         // rotary management struct
    Debounce rdeb;      // rotary debounce/hysteresis struct

    // Initialize PIC chip
    Init();

    // Initialize rotary structs/hardware
    RotaryInit(&rot);
    RotaryReset(&rot);
    RotaryDebounceInit(&rdeb);

    // Loop at ITERS_PER_SEC
    //     If ITERS_PER_SEC is 125, this is an 8msec loop
    //
    while (1) {
        // Sample input ports all at once
        SampleInputs();

        // Keep the millisecond counter running..
        G_msec = (G_msec + G_msecs_per_iter) % 1000;   // wrap at 1000

        // Keep status status led flashing
        FlashCpuStatusLED();

/*** DEBUG
        // USE SCOPE TO DEBUG Debounce OF ROTARY
        //     1. Attach +12 to card, and rotary phone on EXT *2*, pickup ICM line
        //     2. Put scope on pin #10 to monitor cleaned up signal
        //     3. To include 'iterations' square wave, connect pin#3 to scope with a series 220ohm,
        //        to see pin#3 squarewave on top of pin#10's cleaned up signal, e.g.
        //
        //                        220
        //         (pin 3) o-----/\/\/\----O------- (scope lead)
        //                                 |
        //        (pin 10) o---------------+
        //
        // Scope result:                        space
        //                                      <---->
        //                    _-_-_-_-_-_-_-_-_       -_-_-_-_-_-_-_-_-_
        //                   |                 |      |                 |
        //                   |  <---pulse--->  |      |                 |
        //        _-_-_-_-_-_|                 |-_-_-_|                 |-_-_-_-_-_-
        //
        static long G_iters = 0;                 // number of main loop iters. wraps to zero every ITERS_PER_SEC.
        int is_rotary_pulse = DebounceNoisyInput(&rdeb, ROTARY_PULSE);
        LATBbits.LATB7      = is_rotary_pulse;   // B7(pin 10): clean version of ROTARY_PULSE
        LATAbits.LATA4      = G_iters & 1;       // A4(pin  3): on/off each iter

        // Loop counter
        if ( ++G_iters > ITERS_PER_SEC ) G_iters = 0;   // wrap to 0 each sec

***/
        if ( (digit = GetDTMFDigit()) )
            G_buzz_ext = digit;                // DTMF dialed digit? Buzz that extension
        else if ( (digit = GetRotaryDigit(&rot, &rdeb)) != -1 )
            G_buzz_ext = digit;                // Rotary dialed digit? Buzz that extension
        else
            G_buzz_ext = -1;                   // ensure buzzer xstrs not left on

        // Loop delay
        __delay_ms(G_msecs_per_iter);

        // Powerup counter
        //     Counts up from zero then stops after 10 secs, leaving counter >=10000.
        if ( G_powerup_msecs < 10000 ) G_powerup_msecs += G_msecs_per_iter;
    }
}
