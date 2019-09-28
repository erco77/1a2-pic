// vim: autoindent tabstop=8 shiftwidth=4 expandtab softtabstop=4

/*
 * File:   main.c
 * Author: Greg Ercolano, erco@seriss.com
 *
 * Created on Apr 24, 2019, 08:22 AM
 * Compiler: MPLAB X IDE V5.10 + XC8 -- Microchip.com
 *
 * Drive the 1A2 Multiline Phone Control board, CPU1 Firmware.
 *                               _    _
 *                           V+ | |__| | GND
 *     L1_RING_DET (IN) -- RA5  |      | RA0 -- (OUT) L1_LAMP (DAT)
 *     L1_LINE_DET (IN) -- RA4  |      | RA1 -- (OUT) L1_HOLD_RLY (CLK)
 *        (MCLR) X (IN) -- RA3  |      | RA2 -- (OUT) RING_GEN_POW
 *      L1_A_SENSE (IN) -- RC5  |      | RC0 -- (OUT) L2 HOLD_RLY
 *      L2_A_SENSE (IN) -- RC4  |      | RC1 -- (OUT) BUZZ_RING
 *   SECONDARY_DET (IN) -- RC3  |      | RC2 -- (OUT) L1_RING_RLY
 *     L2_LINE_DET (IN) -- RC6  |      | RB4 -- (OUT) L2_RING_RLY
 * CPU_STATUS_LED (OUT) -- RC7  |      | RB5 -- (OUT) L2_LAMP
 *     L2_RING_DET (IN) -- RB7  |______| RB6 -- (IN/OUT) SYNC_ILINK
 *
 *                         PIC16F1709 / CPU1
 *                           REV G, G1, H
 *
 * "REV H" differs from "REV G1" in the following ways:
 *    > New input SECONDARY_DET on Cpu1/RC3 (currently unused)
 *    > New input SECONDARY_DET on Cpu2/RA5 (currently unused)
 *    > New onboard fuse
 *
 * "REV G1" differs from "REV G" in the following ways:
 *    > No impact on software at all
 *    > Added TIP125 surge suppression diodes D22-D25
 *    > Change surface mount resistors to through hole (SMT annoying to solder)
 *    > Repositioned R25, R26, and R28 (changed to R27) for easier soldering/lead trimming
 *    > Swapped component numbers for R28 <-> R27: regional sequential numbering
 *    > Moved C7 next to C5 + C6 for easier assembly
 *    > Moved JP5 to left of EXT1 for cable clearence
 *    > Increased INTERLINK connector pad size for *solder side only* (more surface area for soldering)
 *    > Increased overall pad size for JP3 & 4 (more surface area for soldering)
 *    > Slight enlarge of LED holes to allow red status leds to flush mount
 *    > Added extra pad holes for 60/1W and 300/1W resistors
 *
 * "REV G" differs from "REV F" in the following ways:
 *    > REV G added 'SYNC' I/O signal to allow two boards to keep their
 *      flashing signals in sync (Hold + Ring Flash). Mostly an INPUT,
 *      briefly switches to output to send short sync signal to "other" cpu.
 *      This ensures "slower" cpu autosyncs itself to "faster" one.
 *      See UpdateInterlinkSync() function for complete implementation.
 *    > LED through holes enlarged slightly allowing red CPU LEDs to flush mount
 *    > Some traces moved around for clearance issues, but no electrical changes
 * 
 * "REV F" differs from "REV E" in the following ways:
 *    > REV F now has two PIC chips; CPU1 and CPU2, each with separate firmware
 *    > CPU1 handles logic same as REV E's single CPU, but no longer handles MT8870 signals
 *    > CPU2 handles MT8870 signals and rotary dial counting
 *    > HandleDTMF() function removed from CPU1 firmware, moved to CPU2
 */

//                                                      Port(ABC)
//                                   76543210           |Bit# in port
// Inputs                            ||||||||           ||
#define L1_A_SENSE     ((G_portc & 0b00100000)?0:1) // RC5: low when A lead engaged (0:1 instead of 1:0 to undo negative logic)
#define L2_A_SENSE     ((G_portc & 0b00010000)?0:1) // RC4: low when A lead engaged (0:1 instead of 1:0 to undo negative logic)
#define L1_RING_DET    ((G_porta & 0b00100000)?0:1) // RA5: low on ring detect (0:1 instead of 1:0 to undo negative logic)
#define L2_RING_DET    ((G_portb & 0b10000000)?0:1) // RB7: low on ring detect (0:1 instead of 1:0 to undo negative logic)
#define L1_LINE_DET    ((G_porta & 0b00010000)?0:1) // RA4: low on line detect (0:1 instead of 1:0 to undo negative logic)
#define L2_LINE_DET    ((G_portc & 0b01000000)?0:1) // RC6: low on line detect (0:1 instead of 1:0 to undo negative logic)
#define SECONDARY_DET  ((G_portc & 0b00001000)?0:1) // RC3: detects if card configured as SECONDARY (JP4) [currently unused]

// Outputs
#define L1_HOLD_RLY    LATAbits.LATA1               // hi puts L1 on hold
#define L2_HOLD_RLY    LATCbits.LATC0               // hi puts L2 on hold
#define L1_RING_RLY    LATCbits.LATC2               // hi rings L1
#define L2_RING_RLY    LATBbits.LATB4               // hi rings L2
#define RING_GEN_POW   LATAbits.LATA2               // hi supplies +12V to ring generator
#define L1_LAMP        LATAbits.LATA0               // hi turns on L1's lamp on all extensions
#define L2_LAMP        LATBbits.LATB5               // hi turns on L2's lamp on all extensions
#define CPU_STATUS_LED LATCbits.LATC7               // hi turns on CPU STATUS led
#define BUZZ_RING      LATCbits.LATC1               // hi/lo output to buzz phones during incoming calls

// In/Out
#define SYNC_ILINK_OUT LATBbits.LATB6               // RB6[OUT]: pull low when sending sync to "other" cpu
#define SYNC_ILINK_IN  ((G_portb & 0b01000000)?0:1) // RB6[IN]: low when sync sent by "other" cpu

// Ring timers
//
// Terminology:
//
//     Ring Cycle: The 6 second timer since last CO ring; while running, line is ringing;
//                 keeps line lamps flashing and keeps the 1a2 "Ring Sequence" running,
//                 and keeps the 12V "ring generator power" turned on. There is one per line.
//
//     Ring Sequence: The 4 second 1A2 ring sequence; 1 second ring, 3 second pause.
//                    With fixed cadence ringing, there is one ring sequence timer for all lines,
//                    so that when more than one line is ringing, they all ring together.
//
//
// FIXED RINGING
//     A constant regenerated ring pattern is used for the 1A2 extension's bells
//     to ensure a consistent pattern of ringing when more than one line rings at once.
//
// G_int_tmr:
//             0ms    1000ms 2000ms 3000ms 4000ms   <-- note this is a countUP timer..
//             |......|......|......|......|            ..starts at 0 and counts up.
//              ______
//             |      |
//             | RING |_____________________
//
// The ring result:
//
//               1sec   2sec   3sec   4sec   5sec   6sec   7sec
//             |......|......|......|......|......|......|.....
//              ______                      ______
//             |      |                    |      |
//             | RING |____________________| RING |_________..
//
//             :                           :
//             :<---- RING_SEQ_MSECS ----->:
//             :                           :

#define RING_CYCLE_MSECS     6000   // #msecs count for ring cycle (how long to keep lamps flashing)
#define RING_SEQ_MSECS       4000   // #msecs count for ring sequence (used for fixed cadence ringing)

// This must be #defined before #includes
#define _XTAL_FREQ 4000000UL        // system oscillator speed in HZ (__delay_ms() needs this)

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
#include "TimerMsecs.h"             // our TimerMsecs module

// DEFINES
#define uchar unsigned char
#define uint  unsigned int
#define ITERS_PER_SEC    250        // while() loop iters per second (Hz). *MUST BE EVENLY DIVISIBLE INTO 1000*
#define TIMER1_FREQ      31250      // timer1 counts per second
#define TIMER1_ITER_WAIT (TIMER1_FREQ/ITERS_PER_SEC)
                                    // What Timer1 counts up to every iteration

// GLOBALS
const int  G_msecs_per_iter = (1000/ITERS_PER_SEC);  // #msecs per iter (if ITERS_PER_SEC=125, this is 8. If 250, 4)
TimerMsecs    L1_hold_tmr;             // timer for L1 hold sense
TimerMsecs    L2_hold_tmr;             // timer for L2 hold sense
uchar         L1_hold = 0;             // Line1 HOLD state: 1=call on hold, 0=not on hold
uchar         L2_hold = 0;             // Line2 HOLD state: 1=call on hold, 0=not on hold
// Ringing Timers
//     These keep lamps flashing, bells ringing, and RING_GEN_POW enabled during entire ring cycle.
//
TimerMsecs L1_ringing_tmr;             // 6sec ring timer reset by each CO ring. Keeps lamps flashing,
TimerMsecs L2_ringing_tmr;             // and RING_GEN_POWER activated during ringing.
TimerMsecs G_int_tmr;                  // 4sec "interrupter": lamp flash and ringing based on this timer.
                                       // Kept in sync by SYNC signal. counts 0 to 4000, rings during 0-1000.
uchar      G_buzz_signal     = 0;      // 1 indicates isr() should toggle buzzer
char       G_hold_flash      = 0;      // changes at lamp hold flash rate of 2Hz, 80% duty cycle (1=lamp on, 0=off)
char       G_ring_flash      = 0;      // changes at lamp ring flash rate of 1Hz, 50% duty cycle (1=lamp on, 0=off)
int        G_curr_line       = 0;      // "current line" being worked on. Used by HandleLine() and hardware funcs
uchar      G_porta, G_portb, G_portc;  // 8 bit input sample buffers (once per main loop iter)
uint       G_timer1_cnt      = 0;      // running value of main loop Timer1
uint       G_iter            = 1;      // iteration counter (1-250)

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

// Interrupt service routine
//     Handles oscillating BUZZ_RING at hardware controlled rate of speed
//
void __interrupt() isr(void) {
    static char count = 0;
    if ( INTCONbits.TMR0IF ) {                  // int timer overflow?
        INTCONbits.TMR0IF = 0;                  // clear bit for next overflow
        BUZZ_RING = ((++count & 1) && G_buzz_signal) ? 1 : 0;
                                                // flip oscillator on + off
    }
}

// Initialize PIC chip I/O
void Init() {
    OPTION_REGbits.nWPUEN = 0;   // Enable WPUEN (weak pullup enable) by clearing bit

    // Set PIC chip oscillator speed
    OSCCONbits.IRCF   = 0b1101;  // 0000=31kHz LF, 0111=500kHz MF (default on reset), 1011=1MHz HF, 1101=4MHz, 1110=8MHz, 1111=16MHz HF
    OSCCONbits.SPLLEN = 0;       // disable 4xPLL (PLLEN in config words must be OFF)
    OSCCONbits.SCS    = 0b10;    // 10=int osc, 00=FOSC determines oscillator

    // NOTE: in the following TRISA/B/C data direction registers,
    //       '1' configures an input, '0' configures an output.
    //       'X' indicates a don't care/not implemented on this chip hardware.
    //
    TRISA  = 0b00111000; // data direction for port A (0=output, 1=input)
    WPUA   = 0b00111000; // enable 'weak pullup resistors' for all inputs
    //         ||||||||_ A0 (OUT) L1 LAMP
    //         |||||||__ A1 (OUT) L1 HOLD RLY
    //         ||||||___ A2 (OUT) RING GEN POW
    //         |||||____ A3 (IN)  unused/MCLR
    //         ||||_____ A4 (IN) L1 LINE DET
    //         |||______ A5 (IN) L1 RING DET
    //         ||_______ X
    //         |________ X
    
    // WARNING: Any changes to TRISB/WPUB must be reflected in the code for
    //          HandleInterlinkSync() as well.
    //
    TRISB  = 0b11000000; // data direction for port B (0=output, 1=input)
    WPUB   = 0b11000000; // enable 'weak pullup resistors' for all inputs
    //         ||||||||_ X
    //         |||||||__ X
    //         ||||||___ X
    //         |||||____ X
    //         ||||_____ B4 (OUT) L2 RING RLY
    //         |||______ B5 (OUT) L2 LAMP
    //         ||_______ B6 (IN)  SYNC_ILINK
    //         |________ B7 (IN)  L2 RING DET

    TRISC  = 0b01111000; // data direction for port C (0=output, 1=input)
    WPUC   = 0b01110000; // enable 'weak pullup resistors' for all inputs
    //         ||||||||_ C0 (OUT) L2 HOLD RLY
    //         |||||||__ C1 (OUT) BUZZ 60HZ
    //         ||||||___ C2 (OUT) L1 RING RLY
    //         |||||____ C3 (OUT) x
    //         ||||_____ C4 (IN)  L2 A SENSE
    //         |||______ C5 (IN)  L1 A SENSE
    //         ||_______ C6 (IN)  L2 LINE DET
    //         |________ C7 (OUT) CPU STATUS

    // Disable analog stuff
    ANSELA = 0x0;
    ANSELB = 0x0;
    ANSELC = 0x0;
    ADCON0 = 0x0;   // disables ADC

    // Disable slew rate controls
    SLRCONA = 0x0;
    SLRCONB = 0x0;
    SLRCONC = 0x0;

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
        OPTION_REGbits.PS = PS_16;          // Sets prescaler (divisor) to run timer0
        ei();                               // enable ints last (sets up our isr() function to be called by timer interrupts)
    }

    // ENABLE TIMER1 TO COUNT INSTRUCTION CYCLES (FOSC/4)
    //    We use TIMER1 to determine how long to wait each iter
    //    of the main while() loop.
    //
    {
        PIE1bits.TMR1IE     = 0;            // timer1 Interrupt DISABLE (IE)
        PIR1bits.TMR1IF     = 0;            // timer1 Interrupt Flag (IF)

        // See pp.254 for "tmr1 control register"
        // See pp.256 for "summary of regs associated with tmr1"
        //
        //         _________ TMR1CS<1>   \__ Clock Source. LFINTOSC == "Low Freq Internal Oscillator"
        //        | ________ TMR1CS<0>   /   11=LFINTOSC(31kHz), 01=Fosc, 00=Fosc/4
        //        || _______ T1CKPS<1>   \__ Prescaler:
        //        ||| ______ T1CKPS<0>   /   11=1:8, 10=1:4, 01=1:2, 00=1:1
        //        |||| _____ T1OSCEN     LP Oscillator: 1=enable, 0=disable
        //        ||||| ____ T1SYNC      0:sync async clock in w/Fosc, 1=don't
        //        |||||| ___ x           unused
        //        ||||||| __ TMR1ON      1=enable TMR1, 0=disable
        //        ||||||||
        T1CON = 0b11000101;
    }
}

// Set main loop timer1 to specific value
inline void SetTimer1(uint val) {
    // Stop timer before writing to it
    T1CONbits.TMR1ON = 0;

    // Write new values to TMR1H/TMR1L
    TMR1H = (val >> 8);
    TMR1L = val & 0xff;

    // Start timer again
    T1CONbits.TMR1ON = 1;
}

// Reset main loop timer1 to zero
//    Also resets G_timer1_cnt and G_iter
//
inline void ResetTimer1() {
    SetTimer1(0);
    G_timer1_cnt = 0;     // adjust G_timer1_cnt
    G_iter       = 1;     // adjust iter counter
}

// Return the upcounting TMR1 counter as a 16bit unsigned value
//   (We use TMR1 as a free running timer to time the main loop delay)
//
inline uint GetTimer1() {
    int hi, lo;
    do {
        hi = TMR1H;
        lo = TMR1L;
    } while(hi != TMR1H);     // read 2nd time to check timer for wrap
    return (hi << 8) | lo;
}

// Manage the global G_hold_flash variable
//
//     Called from main() while() loop running at ITERS_PER_SEC.
//
//     This global is used by both L1 and L2 to flash the LAMP
//     at the "on HOLD" rate of 2Hz, 80% duty cycle.
//     This variable will be 0 for lamp off, 1 for lamp on.
//
inline void HandleHoldFlash() {
/// // HOLD WINK: 2Hz 80% DUTY CYCLE
/// //
/// //      0               12500 15625           28125 31250
/// //      :               :     :               :     :
/// //      0sec            400ms 500msec         900ms 1000msec
/// //      :               :     :               :     :
/// //       _______________       _______________       _____ _ _ _ ON
/// //      |      "A"      |     |      "B"      |     |
/// // _____|               |_____|               |_____|            OFF
/// //      :               :                           :
/// //      :<------------->:<--->:                     :
/// //      :      80%        20% :                     :
/// //      :                     :                     :
/// //      :<------------------->:                     :
/// //      :       1/2 sec                             :
/// //      :                                           :
/// //      :<----------------------------------------->:
/// //                           1 sec
/// //
/// int count = G_timer1_cnt % TIMER1_FREQ;
/// G_hold_flash = (count <= 12500) ||                // "A"
///                (count >= 15625 && count <= 28125) // "B"
///                ? 1 : 0;


    // We want any interlink sync trimming to happen during
    // the long on-time, and not the short off-time.
    //
    //      0     3125            15625 18750           31250
    //      :     :               :     :               :
    //      0sec  100ms           500ms 600ms           1000msec
    //      :     :               :     :               :
    //     _       _______________       _______________       _____ _ _ _ ON
    //      |     |      "A"      |     |      "B"      |     |
    //      |_____|               |_____|               |_____|            OFF
    //      :     :               :                     :
    //      :<--->:<------------->:                     :
    //      : 20%       80%       :                     :
    //      :                     :                     :
    //      :<------------------->:                     :
    //      :       1/2 sec                             :
    //      :                                           :
    //      :<----------------------------------------->:
    //                           1 sec
    //
    int count = G_timer1_cnt % TIMER1_FREQ;
    G_hold_flash = (count <= 3125) ||                 // "A"
                   (count >= 15625 && count <= 18750) // "B"
                   ? 0 : 1;
}

// Manage the global G_ring_flash variable.
//
//     Called from main() while() loop running at ITERS_PER_SEC.
//
//     This global is used by both L1 and L2 to flash the LAMP
//     at the "RINGING" rate of 1Hz, 50% duty cycle.
//     This variable will be 0 for lamp off, 1 for lamp on.
//
void HandleRingFlash() {
    // RING FLASH: 1Hz 50% DUTY CYCLE
    //
    //      0        15625    31250
    //      :        :        :
    //      0ms      500ms    1000ms
    //      :        :        :
    //       ________          ________ 
    //      |        |        |        |
    // _____|        |________|        |___ _ _ 
    //      :        :        :
    //      :<------>:<------>:
    //      :   50%  :   50%  :
    //      :<--------------->:
    //            1 sec
    //
    int count = G_timer1_cnt % TIMER1_FREQ;
    G_ring_flash = (count <= 15625) ? 1 : 0;
}

// Flash the CPU STATUS led once per second
inline void FlashCpuStatusLED() {
    int count      = G_timer1_cnt % TIMER1_FREQ;
    CPU_STATUS_LED = (count <= 15625) ? 1 : 0;
}

// Change the hardware state of current line's HOLD_RLY
inline void SetHold(char val) {
    switch ( G_curr_line ) {
        case 1: L1_HOLD_RLY = val; L1_hold = val; return;
        case 2: L2_HOLD_RLY = val; L2_hold = val; return;
    }
}

// Change the hardware state of current line's RING_RLY
inline void SetRing(char val) {
    switch ( G_curr_line ) {
        case 1: L1_RING_RLY = val; return;
        case 2: L2_RING_RLY = val; return;
    }
}

// Change the hardware state of current line's LAMP
inline void SetLamp(char val) {
    switch ( G_curr_line ) {
        case 1: L1_LAMP = val; return;
        case 2: L2_LAMP = val; return;
    }
}

// Start the 1/20sec (50msecs) software hold timer value for current line.
inline void StartHoldTimer() {
    switch ( G_curr_line ) {
        case 1: Set_TimerMsecs(&L1_hold_tmr, 50); return;
        case 2: Set_TimerMsecs(&L2_hold_tmr, 50); return;
    }
}

inline void StopHoldTimer() {
    switch ( G_curr_line ) {
        case 1: Stop_TimerMsecs(&L1_hold_tmr); return;
        case 2: Stop_TimerMsecs(&L2_hold_tmr); return;
    }
}

// Is 1/20sec hold timer running?
//    Returns the state of current line's 1/20sec hold timer
//
inline int IsHoldTimer() {
    switch ( G_curr_line ) {
        case 1: return IsRunning_TimerMsecs(&L1_hold_tmr);
        case 2: return IsRunning_TimerMsecs(&L2_hold_tmr);
        default: return 0;   // shouldn't happen
    }
}

// Manage counting one-shot hold timers (if enabled) for current line.
//     Stop timer when it expires.
//
inline void HandleHoldTimer() {
    switch ( G_curr_line ) {
        case 1:
            if ( Advance_TimerMsecs(&L1_hold_tmr, G_msecs_per_iter) ) {
                Stop_TimerMsecs(&L1_hold_tmr);
            }
            return;
        case 2:
            if ( Advance_TimerMsecs(&L2_hold_tmr, G_msecs_per_iter) ) {
                Stop_TimerMsecs(&L2_hold_tmr);
            }
            return;
    }
}

// See if current line's 6sec ring cycle timer is running
inline int IsRingCycle() {
    switch ( G_curr_line ) {
        case 1: return IsRunning_TimerMsecs(&L1_ringing_tmr);
        case 2: return IsRunning_TimerMsecs(&L2_ringing_tmr);
        default: return 0;    // shouldn't happen
    }
}

// See if either line is ringing
inline int IsAnyLineRinging() {
    return( IsRunning_TimerMsecs(&L1_ringing_tmr) |
            IsRunning_TimerMsecs(&L2_ringing_tmr) );
}

// Start the 4sec (4000msec) software ringing timer value for current line
//      This timer keeps lamp flashing between CO rings.
//      This counts in msec.
//
inline void StartRingingTimer() {
    // See if first ring
    //    If so, reset interrupter so first 1a2 ring happens now.
    //
    if ( IsStopped_TimerMsecs(&L1_ringing_tmr) &&
         IsStopped_TimerMsecs(&L2_ringing_tmr) ) {
        Set_TimerMsecs(&G_int_tmr, 4000);  // restart timer
    }
    // Start 6sec ringing timer running
    switch ( G_curr_line ) {
        case 1: Set_TimerMsecs(&L1_ringing_tmr, RING_CYCLE_MSECS); return;
        case 2: Set_TimerMsecs(&L2_ringing_tmr, RING_CYCLE_MSECS); return;
    }
}

// Stop the 6sec software ringing timer value for current line
//     Also stops the ring sequence timer if no lines are ringing.
//
inline void StopRingingTimer() {
    switch ( G_curr_line ) {
        case 1: Stop_TimerMsecs(&L1_ringing_tmr); break;
        case 2: Stop_TimerMsecs(&L2_ringing_tmr); break;
    }
}

// Advance ringing timers
//     Check for expirations and stop.
//
inline void HandleRingingTimers() {
    // Advance L1 timer if running, and stop if timer expired
    if ( Advance_TimerMsecs(&L1_ringing_tmr, G_msecs_per_iter) ) {
        Stop_TimerMsecs(&L1_ringing_tmr);
    }
    // Advance L2 timer if running, and stop if timer expired
    if ( Advance_TimerMsecs(&L2_ringing_tmr, G_msecs_per_iter) ) {
        Stop_TimerMsecs(&L2_ringing_tmr);
    }
    // Advance ring sequence timer, auto-restarts to keep looping
    Advance_TimerMsecs(&G_int_tmr, G_msecs_per_iter);
}

// Return the state of the RING_DET optocoupler with noise removed
inline int IsTelcoRinging(Debounce *d) {
    return (d->value > d->thresh) ? 1 : 0;
}

// Return 1 if bell/buzzer should be ringing or not based on fixed ringing cadence.
inline int IsFixedRinging() {
    return (Get_TimerMsecs(&G_int_tmr) < 1000) ? 1 : 0;
}

// Initialize debounce struct for ring detect input
//
//     'value' range:
//         max_value     30         __________________
//                                 /
//         on thresh     20 ....../...................
//                               /
//         off thresh    10 ..../.....................
//                       0 ____/
//
//
inline void RingDetectDebounceInit(Debounce *d) {
    d->value      = 0;
    d->max_value  = 30;
    d->on_thresh  = 20;
    d->off_thresh = 10;
    d->thresh     = d->on_thresh;
}

// Manage the L1/L2 RING_DET debounce timers
//     Ignore noise false-triggering RING_DET due to capacitive noise from CO lines
//     during pickup/hangup.
//
// Noisey RING_DET Input:
//                         _      ___   _______________       __          _
//                        | | ||||   | |               | ||| |  || |     | |
//             ___________| |_||||   |_|               |_|||||  ||_|_____| |_______
//                       .           . .               .                 .
//                       .<-- Noise -->.               .<---- Noise ---->.
//                       .           . .               .                 .
//                       .           . .               .                 .
//                       .           . .               .                 .
//  Internal RINGDET COUNTER:        . .  _____________.       _         .
//                       .           ^ . /             \      / \/\_     .
//  on thresh  - - - - - - - - - - -/ \./- - - - - - - -\- - / - - -\ - -.- - - - - on thresh
//                       .         /:  v                 \/\/        \   . _
//  off thresh - - - - - - - - - _/-:- - - - - - - - - - - - - - - - -\ -./ \ - - - off thresh
//             ___________/ \_/\/   :                                 :\_/   \____
//                                  :                                 :
//                                  :<-- hits "on" threshold          :<-- hits "off" threshold
//                                  :                                 :
//  IsTelcoRinging():                _________________________________
//                                  |                                 |
//             _____________________|                                 |_________
//
inline void HandleRingDetTimers(Debounce *d1, Debounce *d2) {
    DebounceNoisyInput(d1, L1_RING_DET);
    DebounceNoisyInput(d2, L2_RING_DET);
}

// Initialize debounce struct for A lead input
//
//     'value' range:
//         max_value     10         __________________
//                                 /
//         on thresh     7  ....../...................
//                               /
//         off thresh    4  ..../.....................
//                       0 ____/
//
//
inline void ALeadDebounceInit(Debounce *d) {
    d->value      = 0;
    d->max_value  = 10;
    d->on_thresh  = 7;
    d->off_thresh = 4;
    d->thresh     = d->on_thresh;
}

inline void HandleALeadDebounce(Debounce *d1, Debounce *d2) {
    DebounceNoisyInput(d1, L1_A_SENSE);
    DebounceNoisyInput(d2, L2_A_SENSE);
}

// Return the hardware state of the LINE_DET optocoupler
//     Returns 1 when current is flowing through Tip/Ring, indicating
//     the line is in use; either with an active call, or call on HOLD.
//
inline int IsLineDetect() {
    switch ( G_curr_line ) {
        case 1: return(L1_LINE_DET ? 1 : 0);
        case 2: return(L2_LINE_DET ? 1 : 0);
        default: return 0;       // shouldn't happen
    }
}

// Returns 1 if call is currently on hold, 0 if not
inline int IsHold() {
    switch ( G_curr_line ) {
        case 1: return(L1_hold); // TODO: try reading hardware state (L1_HOLD_RLY)
        case 2: return(L2_hold);
        default: return 0;       // shouldn't happen
    }
}

// Returns 1 if A Lead is engaged
inline int IsALead(Debounce *d) {
    return (d->value > d->thresh) ? 1 : 0;
}

// MANAGE L1/L2 INPUTS/OUTPUTS
//     Set G_curr_line before calling this function to define the line#
//     to be managed during execution.
//
//     In the following, letters in caps (A:, B:, etc) refer to points
//     in the logic diagram file (README-firmware-logic-diagram.txt)
//     included with this .c file.
//
void HandleLine(Debounce *rd, Debounce *ad) {
    // A: Line Detect?
    if ( IsLineDetect() ) {
        // B: Hold?
        if ( IsHold() ) {                  // line currently on HOLD?
            // C: A lead?
            if ( IsALead(ad) ) {           // on hold, but A LEAD now active?
                // D: Pickup From HOLD
                StopHoldTimer();
                StopRingingTimer();
                SetHold(0);                // return to non-hold state
                SetRing(0);                // not ringing
                SetLamp(1);                // line lamp on steady
                return;
            } else {                       // no A lead, still on HOLD
                // E: Call on HOLD
                StopHoldTimer();
                StopRingingTimer();
                SetHold(1);                // stay on hold
                SetRing(0);                // not ringing
                SetLamp(G_hold_flash);     // flash line lamp at HOLD rate
                return;
            }
        } else {
            // F: Active Call
            if ( IsALead(ad) ) {
                // G: On Call -- Line in use
                StopHoldTimer();
                StopRingingTimer();
                SetHold(0);                // not on hold
                SetRing(0);                // not ringing
                SetLamp(1);                // lamp on steady
                return;
            } else {
                // H: Hold or Hangup
                //    Wait 1/20sec: if A lead still gone but Line Det active, call went on HOLD.
                //    Otherwise, if Line Det dropped too, call was hung up.
                //
                if ( ! IsHoldTimer() ) {   // A LEAD /just/ dropped?
                    StartHoldTimer();      // Start 1/20sec hold timer
                    StopRingingTimer();
                    SetHold(0);            // no HOLD yet until verified when 1/20th timer expires
                    SetRing(0);            // not ringing
                    SetLamp(1);            // lamp on steady; still active call, not sure if HOLD yet
                    return;
                }

                // Count hold timer
                HandleHoldTimer();

                // Watch for hold condition when timer expires
                if ( IsHoldTimer() ) {     // HOLD timer still running?
                    //HandleHoldTimer();   // timer handled in main()
                    StopRingingTimer();
                    SetHold(0);            // no HOLD relay yet until verified
                    SetRing(0);            // not ringing
                    SetLamp(1);            // lamp on steady; still active call, not sure if HOLD yet
                    return;
                } else {                   // HOLD timer expired: if we're still here, put call on HOLD.
                    StopHoldTimer();
                    StopRingingTimer();
                    SetHold(1);            // put call on hold
                    SetRing(0);            // not ringing
                    SetLamp(G_hold_flash); // flash line lamp at HOLD rate
                    return;
                }
            }
        }
    } else {
        // I: Idle (no line detect)
        if ( IsTelcoRinging(rd) ) {
            // CO is currently ringing the line?
            // Restart 'line ringing' timer whenever a ring is detected.
            //
            StartRingingTimer();             // Start 6sec ring timer
        }
        // Line has incoming call, either ringing or between rings.
        // Keep lamp blinking between rings, have 1A2 ring relay
        // follow the CO's ring cadence.
        //
        if ( IsRingCycle() ) {
            // J: Line is ringing
            //    Let 6sec ringing counter count to keep lamp flashing between rings,
            //    and let ring relay follow the CO's ringing signal for same cadence.
            //
            StopHoldTimer();
            SetHold(0);
            SetRing(IsFixedRinging());   // manage ring relay
            SetLamp(G_ring_flash);       // flash line lamp at RING rate
            return;
        } else {
            // K: Line idle
            //    Nothing is going on; no ringing, no hold, no call, nuthin.
            //
            StopHoldTimer();
            StopRingingTimer();
            SetHold(0);          // disable HOLD relay
            SetRing(0);          // disable ringing
            SetLamp(0);          // lamp off
            return;
        }
    }
}

// Buffer the hardware state of PIC's PORTA/B/C all at once.
//     Run this at the beginning of each iter of the 125Hz main loop.
//     We then do bit tests on these buffered values, to avoid multiple
//     hardware reads throughout execution to avoid sampling parallax.
//
inline void SampleInputs() {
    // Buffer the hardware input states
    G_porta = PORTA;
    G_portb = PORTB;
    G_portc = PORTC;
}

// Handle flagging isr() to: toggle BUZZ_RING during ringing (or not)
inline void HandleBuzzRing(Debounce *rd1, Debounce *rd2) {
    G_buzz_signal = IsFixedRinging() ? 1 : 0;
}

//
// Manage the sync signal between two CPUs on different boards over interlink.
//
void HandleInterlinkSync(int send_sync) {
    static char sending  = 0;
    static char lastsync = 0;
    char sync;

    if ( send_sync ) {
        sending        = 1;          // Indicate for next iter we're sending sync
        TRISB          = 0b10000000; // Change RB6 to be OUTPUT
        WPUB           = 0b10000000; // Enable 'weak pullup resistors' for all inputs
        SYNC_ILINK_OUT = 0;          // Set output low to send signal, leave low for full iter
        lastsync       = 1;          // force lastsync hi (idle/normal)
    } else {
        if ( sending ) {             // Are we still sending output since last iter?
            sending  = 0;            // Turn off send flag; no longer sending sync
            lastsync = 1;            // force lastsync hi (idle/normal)
////        SYNC_ILINK_OUT = 1;      // Set output hi for signal off      // XXX: when tested, add this
            TRISB    = 0b11000000;   // Set RB6 back to INPUT
            WPUB     = 0b11000000;   // Enable 'weak pullup resistors' for all inputs
            return;
        }
        sync = SYNC_ILINK_IN;        // Snapshot sync input from remote
        if ( sync == 0 && lastsync == 1 ) { // falling edge of sync from remote?
            // Reset hardware timer
            ResetTimer1();
        }
        lastsync = sync;             // keep track of state between iters (to detect edge)
    }
}

// In the following, the capital letters (A,B,C..) refer
// to the large diagram in README--REV-X--logic-diagram.txt file.
//
void main(void) {
    uint timer1_wait = 0;

    // Ring detect debounce/hysteresis struct
    Debounce ringdet_d1, ringdet_d2;

    // A lead debounce/hysteresis struct
    Debounce a_lead_d1, a_lead_d2;

    // Initialize PIC chip
    Init();

    // Initialize L1/L2 ring timers
    Init_TimerMsecs(&L1_ringing_tmr);
    Init_TimerMsecs(&L2_ringing_tmr);

    // Initialize interrupter
    Init_TimerMsecs(&G_int_tmr);
    Set_TimerMsecs(&G_int_tmr, RING_SEQ_MSECS);   // start interrupter running 4 sec cycle

    RingDetectDebounceInit(&ringdet_d1);
    RingDetectDebounceInit(&ringdet_d2);
    ALeadDebounceInit(&a_lead_d1);
    ALeadDebounceInit(&a_lead_d2);

    // Start hardware timer1 at zero
    ResetTimer1();

    // Loop at ITERS_PER_SEC
    //     If ITERS_PER_SEC is 125, this is an 8msec loop
    //
    while (1) {
        // Take snapshot of timer1, reset timer if reaches 1sec count.
        //    G_timer1_cnt is the time base for all flashing, ringing, etc.
        //
        if ( (G_timer1_cnt = GetTimer1()) >= TIMER1_FREQ ) {
            ResetTimer1();
            G_timer1_cnt = 0;
            G_iter       = 1;
            HandleInterlinkSync(1);
        } else {
            HandleInterlinkSync(0);
        }

        // Determine timer count to wait for
        timer1_wait = G_iter * 125;             // 125*250=31250

        // Sample input ports all at once
        SampleInputs();

        // Keep CPU STATUS lamp flashing
        FlashCpuStatusLED();

        // Manage the G_hold_flash variable each iter
        HandleHoldFlash();

        // Manage the G_ring_flash variable each iter
        HandleRingFlash();

        // Manage the A lead inputs
        HandleALeadDebounce(&a_lead_d1, &a_lead_d2);

        // Manage counting the 1/10sec L1/L2_ringdet_timer each iter.
        HandleRingDetTimers(&ringdet_d1, &ringdet_d2);

        // Manage counting the ring related counters
        HandleRingingTimers();

        // Manage buzz ringing
        HandleBuzzRing(&ringdet_d1, &ringdet_d2);

        // Handle logic signals for Line #1 and Line #2
        G_curr_line = 1; HandleLine(&ringdet_d1, &a_lead_d1);
        G_curr_line = 2; HandleLine(&ringdet_d2, &a_lead_d2);

        // Handle ring generator power
        //     This should be 'on' during and between all ringing on L1 or L2,
        //     so we combine the 6sec ringing timers for both lines..
        //
        RING_GEN_POW = IsAnyLineRinging();

        // LOOP DELAY: Wait on hardware timer for iteration delay
        //             This gives accurate main loop iters, no matter speed of code execution
        //
        while ( GetTimer1() < timer1_wait ) { }        // wait until timer reaches iter timer count

        // Wrap iteration counter
        if ( ++G_iter > ITERS_PER_SEC ) G_iter = 1;
    }
}
