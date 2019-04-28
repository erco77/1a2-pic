// vim: autoindent tabstop=8 shiftwidth=4 expandtab softtabstop=4
/*
 * File:   main.c
 * Author: Greg Ercolano, erco@seriss.com
 *
 * Created on Apr 24, 2019, 08:22 AM
 * Compiler: MPLAB X IDE V5.10 + XC8 -- Microchip.com
 *
 * Drive the 1A2 Multiline Phone Control board, REV F - CPU1 Firmware.
 *                               _    _
 *                           V+ | |__| | GND
 *     L1_RING_DET (IN) -- RA5  |      | RA0 -- (OUT) L1_LAMP (DAT)
 *     L1_LINE_DET (IN) -- RA4  |      | RA1 -- (OUT) L1_HOLD_RLY (CLK)
 *        (MCLR) X (IN) -- RA3  |      | RA2 -- (OUT) RING_GEN_POW
 *      L1_A_SENSE (IN) -- RC5  |      | RC0 -- (OUT) L2 HOLD_RLY
 *      L2_A_SENSE (IN) -- RC4  |      | RC1 -- (OUT) BUZZ_RING
 *                    x -- RC3  |      | RC2 -- (OUT) L1_RING_RLY
 *     L2_LINE_DET (IN) -- RC6  |      | RB4 -- (OUT) L2_RING_RLY
 * CPU_STATUS_LED (OUT) -- RC7  |      | RB5 -- (OUT) L2_LAMP
 *     L2_RING_DET (IN) -- RB7  |______| RB6 -- x
 *
 *                         PIC16F1709 / CPU1
 *                               REV F
 *
 * "REV F" differs from "REV E" in the following ways:
 *    > REV F now has two PIC chips; CPU1 and CPU2, each with separate firmware
 *    > CPU1 handles logic same as REV E's single CPU, but no longer handles MT8870 signals
 *    > CPU2 handles MT8870 signals and rotary dial counting
 *    > HandleDTMF() function removed from CPU1 firmware, moved to CPU2
 */

// REVISION F / CPU1                                    Port(ABC)
//                                   76543210           |Bit# in port
// Inputs                            ||||||||           ||
#define L1_A_SENSE     ((G_portc & 0b00100000)?0:1) // RC5: low when A lead engaged (0:1 instead of 1:0 to undo negative logic)
#define L2_A_SENSE     ((G_portc & 0b00010000)?0:1) // RC4: low when A lead engaged (0:1 instead of 1:0 to undo negative logic)
#define L1_RING_DET    ((G_porta & 0b00100000)?0:1) // RA5: low on ring detect (0:1 instead of 1:0 to undo negative logic)
#define L2_RING_DET    ((G_portb & 0b10000000)?0:1) // RB7: low on ring detect (0:1 instead of 1:0 to undo negative logic)
#define L1_LINE_DET    ((G_porta & 0b00010000)?0:1) // RA4: low on line detect (0:1 instead of 1:0 to undo negative logic)
#define L2_LINE_DET    ((G_portc & 0b01000000)?0:1) // RC6: low on line detect (0:1 instead of 1:0 to undo negative logic)
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
#include <pic16f1709.h>             // our chip

// DEFINES
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long
#define ITERS_PER_SEC  250           // while() loop iters per second (Hz). *MUST BE EVENLY DIVISIBLE INTO 1000*

// GLOBALS
const uint G_msecs_per_iter = (1000/ITERS_PER_SEC);  // #msecs per iter (if ITERS_PER_SEC=125, this is 8)
const int G_ringdet_max_iters  = 30; // ringdet_timer won't count above this (prevents wrap)
const int G_ringdet_on_thresh  = 20; // ringdet_timer must count above this to "snap on"
const int G_ringdet_off_thresh = 10; // ringdet_timer must count below this to "snap off"
ulong G_msec            = 0;         // Millisec counter; counts up from 0 to 1000, steps by G_msecs_per_iter, wraps to zero.
uchar L1_hold           = 0;         // Line1 HOLD state: 1=call on hold, 0=not on hold
uchar L2_hold           = 0;         // Line2 HOLD state: 1=call on hold, 0=not on hold
uint  L1_hold_timer     = 0;         // countdown timer for hold sense. 0: timer disabled, >=1 timer running
uint  L2_hold_timer     = 0;         // countdown timer for hold sense. 0: timer disabled, >=1 timer running
ulong L1_ringing_timer  = 0;         // countdown timer in msec
ulong L2_ringing_timer  = 0;         // countdown timer in msec
int   L1_ringdet_timer  = 0;         // 1/10sec debounce countdown timer for L1_RING_DET
int   L2_ringdet_timer  = 0;         // 1/10sec debounce countdown timer for L2_RING_DET
int   L1_ringdet_thresh = 20;        // Line1 threshold for 'snap action' (hysteresis) for ring detect (ignores on/off noise)
int   L2_ringdet_thresh = 20;        // Line2 threshold for 'snap action' (hysteresis) for ring detect (ignores on/off noise)
char  G_hold_flash      = 0;         // changes at lamp hold flash rate of 2Hz, 80% duty cycle (1=lamp on, 0=off)
char  G_ring_flash      = 0;         // changes at lamp ring flash rate of 1Hz, 50% duty cycle (1=lamp on, 0=off)
int   G_curr_line       = 0;         // "current line" being worked on. Used by HandleLine() and hardware funcs
uchar G_porta, G_portb, G_portc;     // 8 bit input sample buffers (once per main loop iter)

// Manage the global G_hold_flash variable
//
//     Called from main() while() loop running at ITERS_PER_SEC.
//
//     This global is used by both L1 and L2 to flash the LAMP
//     at the "on HOLD" rate of 2Hz, 80% duty cycle.
//     This variable will be 0 for lamp off, 1 for lamp on.
//
void HandleHoldFlash() {
    // HOLD WINK: 2Hz 80% DUTY CYCLE
    //
    //      0msec                 500msec              1000msec
    //      :     100msec         :    600msec         :
    //      :     :               :    :               :
    // _____       _______________      _______________      __ _ _ _
    //      |     |               |    |               |    |
    //      |_____|               |____|               |____|
    //      :     :               :                    :
    //      :<--> :<------------->:                    :
    //      : 20% :      80%      :                    :
    //      :                     :                    :
    //      :<------------------->:                    :
    //      :       1/2 sec                            :
    //      :                                          :
    //      :<---------------------------------------->:
    //                           1 sec
    //
    G_hold_flash = ( G_msec >= 100 && G_msec <= 500  ) ||
                   ( G_msec >= 600 && G_msec <= 1000 ) ? 1 : 0;
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
    //      0msec    500msec
    //      :        :
    // _____          ________          ________
    //      |        |        |        |
    //      |________|        |________|
    //      :        :        :
    //      :<------>:<------>:
    //      :   50%  :   50%  :
    //      :<--------------->:
    //            1 sec
    //
    G_ring_flash = ( G_msec >= 500 ) ? 1 : 0;
}

// Initialize PIC chip I/O for REV E board.
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

    TRISB  = 0b10000000; // data direction for port B (0=output, 1=input)
    WPUB   = 0b10000000; // enable 'weak pullup resistors' for all inputs
    //         ||||||||_ X
    //         |||||||__ X
    //         ||||||___ X
    //         |||||____ X
    //         ||||_____ B4 (OUT) L2 RING RLY
    //         |||______ B5 (OUT) L2 LAMP
    //         ||_______ B6 (OUT) x
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
}

// Flash the CPU STATUS led once per second
void FlashCpuStatusLED() {
    CPU_STATUS_LED = ( G_msec >= 500 ) ? 1 : 0; // Blink LED 1Hz, 50% duty
}

// Change the hardware state of current line's HOLD_RLY
void SetHold(char val) {
    switch ( G_curr_line ) {
        case 1: L1_HOLD_RLY = val; L1_hold = val; return;
        case 2: L2_HOLD_RLY = val; L2_hold = val; return;
    }
}

// Change the hardware state of current line's RING_RLY
void SetRing(char val) {
    switch ( G_curr_line ) {
        case 1: L1_RING_RLY = val; return;
        case 2: L2_RING_RLY = val; return;
    }
}

// Change the hardware state of current line's LAMP
void SetLamp(char val) {
    switch ( G_curr_line ) {
        case 1: L1_LAMP = val; return;
        case 2: L2_LAMP = val; return;
    }
}

// Start the 1/20sec (50msecs) software hold timer value for current line.
void StartHoldTimer() {
    switch ( G_curr_line ) {
        case 1: L1_hold_timer = 50; return;
        case 2: L2_hold_timer = 50; return;
    }
}

void StopHoldTimer() {
    switch ( G_curr_line ) {
        case 1: L1_hold_timer = 0; return;
        case 2: L2_hold_timer = 0; return;
    }
}

// Return the state of current line's 1/20sec hold timer
int IsHoldTimer() {
    switch ( G_curr_line ) {
        case 1: return(L1_hold_timer ? 1 : 0);
        case 2: return(L2_hold_timer ? 1 : 0);
        default: return 0;   // shouldn't happen
    }
}

// Manage counting down software hold timers (if enabled) for current line.
//     Since we count down in msecs, make sure we reach exactly zero.
void HandleHoldTimer() {
    switch ( G_curr_line ) {
        case 1:
            L1_hold_timer = ( L1_hold_timer > G_msecs_per_iter)
                                ? (L1_hold_timer - G_msecs_per_iter)
                                : 0;
            return;
        case 2:
            L2_hold_timer = ( L2_hold_timer > G_msecs_per_iter)
                                ? (L2_hold_timer - G_msecs_per_iter)
                                : 0;
            return;
    }
}

// Start the 6sec (6000msec) software ringing timer value for current line
//      This timer keeps lamp flashing between CO rings.
//      This counts in msec.
//
void StartRingingTimer() {
    switch ( G_curr_line ) {
        case 1: L1_ringing_timer = 6000; return;  // 6000msec == 6sec
        case 2: L2_ringing_timer = 6000; return;  // 6000msec == 6sec
    }
}

// Stop the 6sec software ringing timer value for current line
void StopRingingTimer() {
    switch ( G_curr_line ) {
        case 1: L1_ringing_timer = 0; return;
        case 2: L2_ringing_timer = 0; return;
    }
}

// See if software 6sec ringing timer is running for current line
int IsRingingTimer() {
    switch ( G_curr_line ) {
        case 1: return(L1_ringing_timer ? 1 : 0);
        case 2: return(L2_ringing_timer ? 1 : 0);
        default: return 0;    // shouldn't happen
    }
}

// Countdown the 6sec L1/L2 ringing timers if enabled
void HandleRingingTimers() {
    if ( L1_ringing_timer > 0 ) L1_ringing_timer -= G_msecs_per_iter;
    if ( L2_ringing_timer > 0 ) L2_ringing_timer -= G_msecs_per_iter;
}

// Return the hardware state of the RING_DET optocoupler
//     Read the state of the "software capacitor" with hysteresis.
//
int IsRinging() {
    switch ( G_curr_line ) {
        case 1: return( (L1_ringdet_timer > L1_ringdet_thresh) ? 1 : 0);
        case 2: return( (L2_ringdet_timer > L2_ringdet_thresh) ? 1 : 0);
        default: return 0;                                         // shouldn't happen
    }
}

// Manage the L1/L2 RING_DET debounce timers
//     Ignore noise false-triggering RING_DET due to capacitive noise from CO lines
//     during pickup/hangup. Must have multiple reads of logic high or low in 
//     in a row before IsRinging() changes state. Use a timer to act as an integrating cap,
//     and use an on/off threshold for snap-action hysteresis; when on threshold is reached,
//     counter now must get below the "off threshold" to turn off again. Once off, counter
//     must get above the "on threshold" to turn on again.
//
//     Count up when ON detected (max:G_ringdet_max_iters), count down when OFF detected (min:0).
//     Call once per main loop iter to limit sampling of RING_DET to once per iter.
//
//                         _      ___   _______________       __          _
//  RING_DET:             | | ||||   | |               | ||| |  || |     | |
//             ___________| |_||||   |_|               |_|||||  ||_|_____| |_______
//                       .           . .               .                 .
//                       .<-- Noise -->.               .<---- Noise ---->.
//                       .           . .               .                 .
//                       .           . .
//  RINGDET COUNTER:                 .    _____________        _
//                                   ^   /             \      / \/\_
//  on thresh  - - - - - - - - - - -/ \ /- - - - - - - -\/\/\/ - - -\ - - - - - - - on thresh
//                                 / . v                             \     _
//  off thresh - - - - - - - - - _/- . - - - - - - - - - - - - - - - -\ - / \ - - - off thresh
//             ___________/ \_/\/    .                               . \_/   \____
//                                   .<-- hits "on" threshold        .
//                                   .                               .<-- hits "off" threshold
//                                   ._______________________________.
//  IsRinging():                     |                               |
//             ______________________|                               |__________
//
void HandleRingDetTimers() {
    // LINE #1
    if ( L1_RING_DET ) {
        // RING_DET bit on? Might be noise, debounce..
        if ( L1_ringdet_timer < G_ringdet_max_iters ) {
            if ( ++L1_ringdet_timer > G_ringdet_on_thresh ) {
                // SNAP ON. No longer "off" until above off_thresh
                L1_ringdet_thresh = G_ringdet_off_thresh;
            }
        }
    } else {
        // RING_DET hardware bit OFF? Might be noise, debounce..
        if ( L1_ringdet_timer > 0 ) {
            if ( --L1_ringdet_timer < G_ringdet_off_thresh ) {
                // SNAP OFF. No longer "on" until above on_thresh
                L1_ringdet_thresh = G_ringdet_on_thresh;
            }
        }
    }
    // LINE #2
    if ( L2_RING_DET ) {
        // RING_DET bit on? Might be noise, debounce..
        if ( L2_ringdet_timer < G_ringdet_max_iters ) {
            if ( ++L2_ringdet_timer > G_ringdet_on_thresh ) {
                // SNAP ON. No longer "off" until above off_thresh
                L2_ringdet_thresh = G_ringdet_off_thresh;
            }
        }
    } else {
        // RING_DET hardware bit OFF? Might be noise, debounce..
        if ( L2_ringdet_timer > 0 ) {
            if ( --L2_ringdet_timer < G_ringdet_off_thresh ) {
                // SNAP OFF. No longer "on" until above on_thresh
                L2_ringdet_thresh = G_ringdet_on_thresh;
            }
        }
    }
}

// Return the hardware state of the LINE_DET optocoupler
//     Returns 1 when current is flowing through Tip/Ring, indicating
//     the line is in use; either with an active call, or call on HOLD.
//
int IsLineDetect() {
    switch ( G_curr_line ) {
        case 1: return(L1_LINE_DET ? 1 : 0);
        case 2: return(L2_LINE_DET ? 1 : 0);
        default: return 0;       // shouldn't happen
    }
}

// Returns 1 if call is currently on hold, 0 if not
int IsHold() {
    switch ( G_curr_line ) {
        case 1: return(L1_hold); // TODO: try reading hardware state (L1_HOLD_RLY)
        case 2: return(L2_hold);
        default: return 0;       // shouldn't happen
    }
}

// Returns 1 if A Lead is engaged
int IsALead() {
    switch ( G_curr_line ) {
        case 1: return L1_A_SENSE;
        case 2: return L2_A_SENSE;
        default: return 0;       // shouldn't happen
    }
}

// MANAGE L1/L2 INPUTS/OUTPUTS
//     Set G_curr_line before calling this function to define the line#
//     to be managed during execution.
//
//     In the following, letters in caps (A:, B:, etc) refer to points
//     in the logic diagram file (README-firmware-logic-diagram.txt)
//     included with this .c file.
//
void HandleLine() {
    // A: Line Detect?
    if ( IsLineDetect() ) {
        // B: Hold?
        if ( IsHold() ) {                  // line currently on HOLD?
            // C: A lead?
            if ( IsALead() ) {             // on hold, but A LEAD now active?
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
            if ( IsALead() ) {
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

                // Count down hold timer
                HandleHoldTimer();

                // Watch for hold condition when timer expires
                if ( IsHoldTimer() ) {     // HOLD timer still running?
                    //HandleHoldTimer();   // timer countdown handled in main()
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
        if ( IsRinging() ) {
            // CO is currently ringing the line?
            // Restart 'line ringing' timer whenever a ring is detected.
            //
            StartRingingTimer();             // Start 6sec ring timer
        }
        // Line has incoming call, either ringing or between rings.
        // Keep lamp blinking between rings, have 1A2 ring relay
        // follow the CO's ring cadence.
        //
        if ( IsRingingTimer() ) {
            // J: Line is ringing
            //    Let 6sec ringing counter count down to keep lamp flashing between rings,
            //    and let ring relay follow the CO's ringing signal for same cadence.
            //
            StopHoldTimer();
            //HandleRingingTimer();    // 6sec ringing timer countdown managed in main())
            SetHold(0);
            SetRing(IsRinging());      // have 1A2 ringing follow debounced CO ringing
            SetLamp(G_ring_flash);     // flash line lamp at RING rate
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
void SampleInputs() {
    // Buffer the hardware input states
    G_porta = PORTA;
    G_portb = PORTB;
    G_portc = PORTC;
}

// Run the BUZZ_RING output used to buzz extensions when there's an incoming call
void HandleBuzzRing() {
    static uchar bz_count = 0;
    // Oscillate BUZZ_RING output if Line#1 or Line#2 ringing
    if ( L1_ringdet_timer || L2_ringdet_timer ) {
        BUZZ_RING = (bz_count & 2) ? 1 : 0;         // run buzzer at a lower freq (~30Hz) for "ringing"
        ++bz_count;                                 // run our "oscillator" counter
    } else {
        BUZZ_RING = 0;                              // don't oscillate if no line ringing
    }
}

// In the following, the capital letters (A,B,C..) refer
// to the large diagram in README--REV-X--logic-diagram.txt file.
//
void main(void) {
    // Initialize PIC chip
    Init();

    // Loop at ITERS_PER_SEC
    //     If ITERS_PER_SEC is 125, this is an 8msec loop
    //
    while (1) {
        // Sample input ports all at once
        SampleInputs();
        
        // Keep the millisecond counter running..
        G_msec = (G_msec + G_msecs_per_iter) % 1000;   // wrap at 1000

        // Keep CPU STATUS lamp flashing
        FlashCpuStatusLED();

        // Manage the G_hold_flash variable each iter
        HandleHoldFlash();

        // Manage the G_ring_flash variable each iter
        HandleRingFlash();

        // Manage counting down the 1/10sec L1/L2_ringdet_timer each iter.
        HandleRingDetTimers();

        // Manage counting down the 6sec L1/L2 ringing timer
        HandleRingingTimers();
        
        // Oscillate the BUZZ_RING output
        HandleBuzzRing();
        
        // Handle logic signals for Line #1 and Line #2
        G_curr_line = 1; HandleLine();
        G_curr_line = 2; HandleLine();

        // Handle ring generator power
        //     This should be 'on' during and between all ringing on L1 or L2,
        //     so we combine the 6sec ringing timers for both lines..
        //
        RING_GEN_POW = (L1_ringing_timer | L2_ringing_timer) ? 1 : 0;

        // Loop delay
        __delay_ms(G_msecs_per_iter);
    }
}
