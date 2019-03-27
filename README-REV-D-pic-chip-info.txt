PIC CHIP PROGRAMMER: Ref: Hardware: https://www.youtube.com/watch?v=ksYe_FFAlEM
                          Software: https://www.youtube.com/watch?v=TIsiRmGVgUI


PICKIT 3 PROGRAMMING WIRING
                                                               _    _
            PIN1 >  o-----o MCLR              PIN2 -->     V+ | |__| | GND  <-- PIN3
               2    o-----o V+                             x  |      | DAT  <-- PIN4
               3    o-----o V-/GND                         x  |      | CLK  <-- PIN5
               4    o-----o PGM DATA          PIN1 -->  MCLR  |      | x
               5    o-----o PGM CLK                        x  |      | x
               6    o-----o AUX (don't connect)            x  |      | x
                                                           x  |      | x
                                                           x  |      | x
                                                           x  |      | x
                                                           x  |______| x

                                                             PIC16F1709

               PIC16F1709 COMPLETE PINOUT                    1A2 PIC BOARD ASSIGNMENTS (02/28/2019)
               ==========================                    ======================================
                           _    _                                                      _    _
                       V+ | |__| | GND                                             V+ | |__| | GND 
                     RA5  |      | DAT / RA0                 L1 RING DET (IN) -- RA5  |      | RA0 -- (OUT) L1 LAMP (DAT)
                ____ RA4  |      | CLK / RA1                 L1 LINE DET (IN) -- RA4  |      | RA1 -- (OUT) L1 HOLD (CLK)
                MCLR/RA3  |      | RA2                     (MCLR) unused (IN) -- RA3  |      | RA2 -- (OUT) L2 RING
                     RC5  |      | RC0                         L1 A LEAD (IN) -- RC5  |      | RC0 -- (OUT) L2 HOLD
                     RC4  |      | RC1                         L2 A LEAD (IN) -- RC4  |      | RC1 -- (OUT) L1 RING
                     RC3  |      | RC2                       L2 LINE DET (IN) -- RC3  |      | RC2 -- (OUT) BUZZ 60HZ
                     RC6  |      | RB4                       L2 RING DET (IN) -- RC6  |      | RB4 -- (OUT) L2 LAMP
                     RC7  |      | RB5                       CPU STATUS (OUT) -- RC7  |      | RB5 -- (IN)  "StD"
                     RB7  |______| RB6                            unused (IN) -- RB7  |______| RB6 -- (OUT) "TOE"
                                                          
                         PIC16F1709                       

        Software: MikroC PRO for PIC v.5.4.0 <-- GUI used in video
                  Also: MicroChip recommends "MPLAB IDE Software"

        Xtal: 32.768kHz tuning fork type crystals for LP oscillator mode.
              Connect between SOSCO and SOSCI pins.

        Or, don't use any xtal, and just use the internal oscillator
        by setting the config pragma at the top of the code:

                #pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)

        Reference Manual: http://ww1.microchip.com/downloads/en/DeviceDoc/40001729C.pdf

             p. 355 covers the PICKit programmer pinout (shown above)

             p. 2 gives overview of internal clock oscillator:
                 Clocking Structure
                 * 16 MHz Internal Oscillator Block:
                 * +/- 1% at calibration
                 * Selectable frequency range from 0 to 32 MHz
                 * 31 kHz Low-Power Internal Oscillator
                 * External Oscillator Block with:
                 * Three crystal/resonator modes up to 20 MHz
                 * Two external clock modes up to 20 MHz
                 * Fail-Safe Clock Monitor
                 * Two-Speed Oscillator Start-up
                 * Oscillator Start-up Timer (OST)

             p. 61-62 (Section 6.0 - 6.2) goes into more detail about the clock:

                 6.0 OSCILLATOR MODULE (WITH FAIL-SAFE CLOCK MONITOR)
                 ----------------------------------------------------

                     6.1 Overview
                     ------------
                     The oscillator module has a wide variety of clock sources and
                     selection features that allow it to be used in a wide range of
                     applications while maximizing performance and minimizing power
                     consumption. Figure 6-1 illustrates a block diagram of the oscillator
                     module.  Clock sources can be supplied from external oscillators,
                     quartz crystal resonators, ceramic resonators and Resistor-Capacitor
                     (RC) circuits. In addition, the system clock source can be supplied
                     from one of two internal oscillators and PLL circuits, with a choice
                     of speeds selectable via software. Additional clock features include:

                         * Selectable system clock source between external or internal sources via software.

                         * Two-Speed Start-up mode, which minimizes latency between external oscillator start-up and code execution.

                         * Fail-Safe Clock Monitor (FSCM) designed to detect a failure of the external clock source
                           (LP, XT, HS, ECH, ECM, ECL or EXTRC modes) and switch automatically to the internal oscillator.

                         * Oscillator Start-up Timer (OST) ensures stability of crystal oscillator sources.

                     The oscillator module can be configured in one of the following clock modes.

                         1. ECL - External Clock Low-Power mode (0 MHz to 0.5 MHz)
                         2. ECM - External Clock Medium-Power mode (0.5 MHz to 4 MHz)
                         3. ECH - External Clock High-Power mode (4 MHz to 32 MHz)
                         4. LP - 32 kHz Low-Power Crystal mode.
                         5. XT - Medium Gain Crystal or Ceramic Resonator Oscillator mode (up to 4 MHz)
                         6. HS - High Gain Crystal or Ceramic Resonator mode (4 MHz to 20 MHz)
                         7. EXTRC - External Resistor-Capacitor
                         8. INTOSC - Internal oscillator (31 kHz to 32 MHz)    *** INTERNAL OSCILLATOR RANGE ***

                     Clock Source modes are selected by the FOSC<2:0> bits in the Configuration Words.
                     The FOSC bits determine the type of oscillator that will be used when the device
                     is first powered.

                     The ECH, ECM, and ECL clock modes rely on an external logic level signal
                     as the device clock source.

                     The LP, XT, and HS clock modes require an external crystal or resonator
                     to be connected to the device.

                     Each mode is optimized for a different frequency range.
                     The EXTRC clock mode requires an external resistor and capacitor to set the oscillator frequency.
                     The INTOSC internal oscillator block produces low, medium, and high-frequency clock sources,
                     designated LFINTOSC, MFINTOSC and HFINTOSC.  (see Internal Oscillator Block, Figure 6-1).
                     A wide selection of device clock frequencies may be derived from these three clock sources.

                     ...

                     6.2 Clock Source Types
                     ----------------------
                     Clock sources can be classified as external or internal.

                     External clock sources rely on external circuitry for the
                     clock source to function. Examples are: oscillator modules
                     (ECH, ECM, ECL mode), quartz crystal resonators or ceramic resonators
                     (LP, XT and HS modes) and Resistor-Capacitor (EXTRC) mode circuits.

                     Internal clock sources are contained within the
                     oscillator module. The internal oscillator block has two
                     internal oscillators and a dedicated Phase Lock Loop
                     (HFPLL) that are used to generate three internal
                     system clock sources: the 16 MHz High-Frequency
                     Internal Oscillator (HFINTOSC), 500 kHz (MFINTOSC)
                     and the 31 kHz Low-Frequency Internal Oscillator (LFINTOSC).

                     The system clock can be selected between external or internal clock
                     sources via the System Clock Select (SCS) bits in the OSCCON register.
                     See Section 6.3 "Clock Switching" for additional information. 



             p. 49 covers clock config:

                 REGISTER 4-1: CONFIG1: CONFIGURATION WORD 1
                 bit 2-0 FOSC<2:0>: Oscillator Selection bits
                     111 = ECH: External Clock, High-Power mode (4-20 MHz): device clock supplied to CLKIN pin
                     110 = ECM: External Clock, Medium-Power mode (0.5-4 MHz): device clock supplied to CLKIN pin
                     101 = ECL: External Clock, Low-Power mode (0-0.5 MHz): device clock supplied to CLKIN pin
                     100 = INTOSC oscillator: I/O function on CLKIN pin
                     011 = EXTRC oscillator: External RC circuit connected to CLKIN pin
                     010 = HS oscillator: High-speed crystal/resonator connected between OSC1 and OSC2 pins
                     001 = XT oscillator: Crystal/resonator connected between OSC1 and OSC2 pins
                     000 = LP oscillator: Low-power crystal connected between OSC1 and OSC2 pins


       1A2 LINE CARD STATE FLOW CHART
       ==============================

       +-----------------------------------------+
       |                                         |
       |                                        / \
       |                                 ______/   \_____
       |                                /                \  yes
       |                               <   Line Detect?   >------------+
       |                                \______     _____/             |
       |                                       \   /                  / \
       |                                        \ / no         ______/   \_____
       |                                         |            /                \  yes
       |                                         |           <       Hold?      >--------------------------------+  (Currently on HOLD)
       |                                        / \           \______     _____/                                 |
       |                                 ______/   \_____            \   /                                      / \
       |                            yes /                \            \ / no                             ______/   \_____
       |                   +-----------<       Ring?      >            |  (On Call)                     /                \  yes
       |    (Line Ringing) |            \______     _____/             |                               <      A Lead?     >-----------+
       |                   |                   \   /                  / \                               \______     _____/            |
       |                   |                    \ / no         ______/   \_____                                \   /                  | (Pickup held call)
       |                   |                     |  (Idle)    /                \ yes                            \ / no                |
       |                   |                     |           <      A Lead?     >-----------+                    |  (On HOLD)         |
       |                   |                     |            \______     _____/            | (On Call)          |                    |
       |                   |                     |                   \   /                  |                    |                    |
       |                   |                     |                    \ / no                |                    |                    |
       |                   |                     |                     |  (Pushed HOLD)     |                    |                    |
       |                  \|/                   \|/                   \|/                  \|/                  \|/                  \|/
       |              Line Ringing           Line Idle        Transition To HOLD       Line In Use         Call On HOLD       Pickup From HOLD
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |           |  0 -> Hold Rly  |   |  0 -> Hold Rly  |  |  1 -> Hold Rly  |  |  0 -> Hold Rly  |  |  1 -> Hold Rly  |  |  0 -> Hold Rly  |
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |                   |                     |                     |                    |                    |                    |
       |                  \|/                   \|/                   \|/                  \|/                  \|/                  \|/
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |           |  0 -> Hold State|   |  0 -> Hold State|  |  1 -> Hold State|  |  0 -> Hold State|  |  0 -> Hold State|  |  0 -> Hold State|
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |                   |                     |                     |                    |                    |                    |
       |                  \|/                   \|/                   \|/                  \|/                  \|/                  \|/
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |           | RCAD -> Ring Rly|   |  0 -> Ring Rly  |  |  0 -> Ring Rly  |  |  0 -> Ring Rly  |  |  0 -> Ring Rly  |  |  0 -> Ring Rly  |
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |                   |                     |                     |                    |                    |                    |
       |                  \|/                   \|/                   \|/                  \|/                  \|/                  \|/
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |           | Ring -> Lamp    |   |  0 -> Lamp      |  | Hold -> Lamp    |  |  1 -> Lamp Xstr |  |  Hold -> Lamp   |  |  1 -> Lamp Xstr |
       |           +-----------------+   +-----------------+  +-----------------+  +-----------------+  +-----------------+  +-----------------+  
       |                   |                     |                     |                    |                    |                    |
       |                  \|/                   \|/                   \|/                  \|/                  \|/                  \|/
       |                   |                     |                     |                    |                    |                    |
       +-------------------o---------------------o---------------------o--------------------o--------------------o--------------------o

typedef struct {
    // Inputs (debounced)
    char line_det  = 0;
    char ring_ret  = 0;
    char a_lead    = 0;
    // Outputs
    char hold_rly  = 0;
    char ring_rly  = 0;
    char lamp_xstr = 0;
    // State
    char hold_state = 0;
} Line;

// Lamp cadences
char hold_lamp[] = { 0,1,1,1,1,1,0,1,1,1,1,1 }; // 1 sec overall (~83 msec each)
char ring_lamp[] = { 0,0,0,0,0,0,1,1,1,1,1,1 }; // 1 sec overall (~83 msec each)
char ring_bell[] = { 1,1,0,0,0,0 };             // 6 sec overall (1 sec each)

// Handle input transitions. Reads the 3 inputs: line detect, ring detect, and A lead.
// Given that, manage the outputs: lamp, hold relay, ring relay.
//
void HandleLineTransitions(line) {
   if ( line->line_det ) {
       // Call in progress
       if ( line->hold_state ) {
           // Call On HOLD
           if ( line->a_lead ) {
               // Started picking up a HOLD call
               line->hold_rly   = 0;
               line->hold_state = 0;
               line->ring_rly   = 0;
               line->lamp_xstr  = hold_lamp[counter % sizeof(hold_lamp)];
           } else {
               // Still on HOLD
               line->hold_rly   = 1;
               line->hold_state = 0;
               line->ring_rly   = 0;
               line->lamp_xstr  = hold_lamp[counter % sizeof(hold_lamp)];
           }
       } else {
           // On call (not HOLD)
           if ( line->a_lead ) {
               // On call/line in use
               line->hold_rly   = 0;
               line->hold_state = 0;
               line->ring_rly   = 0;
               line->lamp_xstr  = 1;
           } else {
               // Transition to HOLD (A lead just released)
               line->hold_rly   = 1;
               line->hold_state = 1;
               line->ring_rly   = 0;
               line->lamp_xstr  = hold_lamp[counter % sizeof(hold_lamp)];
           }
       }
   } else {
       // Here if: idle, ringing, dropped from hold, someone hung up, CPC
       if ( line->ring ) {
           // Line ringing
           hold_rly   = 0;
           hold_state = 0;
           ring_rly   = ring_bell[counter % sizeof(ring_bell));
           lamp_xstr  = ring_lamp[counter % sizeof(ring_lamp)];
       } else {
           // Line idle
           hold_rly   = 0;
           hold_state = 0;
           ring_rly   = 0;
           lamp_xstr  = 0;
       }
   }
}




   MPLAB
   =====
   C compiler:
   https://www.microchip.com/mplab/compilers

   MikroC
   ======
   https://www.mikroe.com/blog/compiler-quick-start-guide

   Perhaps a way to get MikroC to write .HEX file to PICKit3:
   http://www.theengineeringprojects.com/2013/03/how-to-burn-mikroc-code-using-pickit3.html



---------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------

    After much histrionics that made the cat leave the room,
    I got two LEDs blinking properly with my PIC16F1709's using
    Microchip's MPLABX IDE environment.

    Bought the PIC chips and a "PICKit 3" programmer from Digikey,
    and the rest was free software downloads of Microchip's IDE.

    The led flashing code ended up being:
    _______________________________________________________________________________________________________________

    /*
     * File:   main.c
     * Author: erco
     *
     * Created on January 6, 2019, 2:37 PM
     */

    #define _XTAL_FREQ 1000000UL        // 4MHz/4?

    // The following a copy/paste from MPLABX's menu: Production -> Set Configuration Bits -> Generate Source Code to Output

    // CONFIG1
    #pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
    #pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
    #pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
    #pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
    #pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
    #pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
    #pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
    #pragma config IESO = OFF       // Internal/External Switchover Mode (Internal/External Switchover Mode is disabled)
    #pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

    // CONFIG2
    #pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
    #pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software)
    #pragma config ZCDDIS = ON      // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR)
    #pragma config PLLEN = OFF      // Phase Lock Loop enable (4x PLL is enabled when software sets the SPLLEN bit)
    #pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
    #pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
    #pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
    #pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

    // #pragma config statements should precede project file includes.
    // Use project enums instead of #define for ON and OFF.

    // Ref: https://electronics.stackexchange.com/questions/171530/pic-microchip-keeps-resetting
    #include <xc.h>
    #include <pic16f1709.h>

    void main(void) {
        TRISA = 0x0;
        TRISB = 0x0;
        TRISC = 0x0;
        while (1) {
            PORTB = 0xaa;  __delay_ms(500);
            PORTB = 0x55;  __delay_ms(500);
        }
    }
    _______________________________________________________________________________________________________________

    ..and the LED wiring ended up being:

                                       _    _
                      5VDC <------ V+ | |__| | GND --------> GND
                                 RA5  |      | DAT / RA0
                            ____ RA4  |      | CLK / RA1
        5VDC <--(47k RES)-- MCLR/RA3  |      | RA2
                                 RC5  |      | RC0
                                 RC4  |      | RC1
                                 RC3  |      | RC2
                                 RC6  |      | RB4
                                 RC7  |      | RB5 --------(LED)-----(220 OHM RES)-----> GND
                                 RB7  |______| RB6 --------(LED)-----(220 OHM RES)-----> GND

                                     PIC16F1709

    ..where the LEDs flash in an alternating pattern.
    And the programmer wiring (overlayed with the above) being:
     
                PICKit 3 PROGRAMMER PINOUT
                ==========================
                                                                     _    _
                PIN1 >  o-----o MCLR                PIN2 -->     V+ | |__| | GND  <-- PIN3
                   2    o-----o V+                               x  |      | DAT  <-- PIN4
                   3    o-----o V-/GND                           x  |      | CLK  <-- PIN5
                   4    o-----o PGM DATA            PIN1 -->  MCLR  |      | x
                   5    o-----o PGM CLK                          x  |      | x
                   6    o-----o AUX (don't connect)              x  |      | x
                                                                 x  |      | x
                                                                 x  |      | x
                                                                 x  |      | x
                                                                 x  |______| x

                                                                   PIC16F1709

    <digression>

    Most of the hassle for me was the Mac version of MPLABX was constantly
    crashing.

    I finally moved to the Windows version which didn't crash, but used
    so much ram it ran really slow on my 2 Gig Windows 8 machine.

    Also, it didn't help that the 16F1709 is a relatively new chip,
    so there weren't any simple "hello world" examples for it with this
    compiler.

    So I was floundering with that for a while, getting weird behavior
    before I found the proper configuration bits needed to get consistent
    behavior, as the chip was auto-resetting (fixed by WDTE=OFF), and timing
    was bad due to the PLL (phase locked loop) modules being enabled by default
    (fixed by PLLEN=OFF).

    While I ended up using Microchip's bloated Java based MPLABX IDE
    and their XC8 compiler (separate download), I'd actually wanted
    to use the MikroC PIC compiler instead, which has great online help
    and a fast/small IDE. But they didn't have an easy way to invoke
    the PICKit 3 programmer to upload the code to the chip.

    Apparently the approach was to configure MikroC to invoke Microchip's
    "standalone programmer", or "PIC Kit Programmer", which was simple
    and small and easy, but Microchip says they stopped supporting it
    in favor of their massive "MPLAB IPE" instead, another bloated
    java application similar to the "MPLAB IDE".

    So all the standalone PIC programmer apps I could find from Microchip
    were frozen in time, old enough not to include my relatively new chip
    in their pulldown menu list of devices :/

    So I ended up sticking with Microchip's MPLAB IDE + Compiler,
    and rewrote my MikroC code to use their programming model instead.
    (Different macros, bad docs.. bleh)
     
---------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------
1A2 PIC 
    
    --- debounce:
    Set up a 100hz interrupt and sample the pin every time. Every time
    you see it high inc a counter, When its low zero it. Once you see
    the counter over x then set another flag to say the input was pressed.

    --- "weak pullup":
    PIC internal "weak pull up" resistors are ~20k.  From the data sheet:
        """
        D070 IPURB PORTB Weak Pull-up Current 50 250 400 uA VDD = 5V,
                         VPIN = VSS, -40 deg C TO +85 deg C
        """
    ..The typical value of 250 microamps is equivalent to a pull up
    resistance of 5/.00025 Ohms = 20K......maybe a bit on the higher side
    for an EMI prone application.

    Inputs:
        Line Detect
        Ring Detect
        A Lead

    Outputs:
        Lamp
        Hold relay
        Ring relay

    Main Loop
    ---------

        o Hold flash/Ring flash on/off bit
          --------------------------------

          Manage this as a binary integer counter that runs at 1/8 second rate:

                               1 sec
                        |----------------|
                   Bit0: __--__--__--__--__--__--__--__--
                        :                :
                   Bit1: ____----____----____----____----
                        :                :
                   Bit2: ________--------________--------
                        :                :
                   Hold: __------__------__------__------  (Bit0|Bit1)
                        :                :
                   Ring: ________--------________--------  (Bit3)


        o Line Detect
          -----------
          Manage with a state flag that's either IDLE (0) or BUSY(1).
          Debounce before making a change in state by checking for several samples.

          The state will be used for:

              o Lamp status (off/on/flashing during ringing/flashing during HOLD)
              o Hold detection (when A lead drops but line detect still shows BUSY for >.1sec)
              o Stops ringing (audible and ring flash) when someone picks up
              o Releases "Hold" on CPC

          Should be handled as an interrupt, so the chip can sleep most of the time,
          and only be "awake" when a line is "BUSY" (with ringing or line detect/hold)

        o Ring Detect
          -----------
          Handle this by debouncing.

          
          On ring, assuming line detect is IDLE, 
          Should be handled as an interrupt, so the chip can sleep
          most of the time, and only be "awake" when a line is active.

        o Sleep mode: only when:

              "Line Detect" state is IDLE
              "Ringing" state is IDLE.

---
    PIC Related Videos (list of different videos):
    https://microchip.secure.force.com/microchipknowledge/articles/en_US/FAQ/Learning-PIC-Microcontrollers-Videos/?l=en_US&fs=Search&pn=1

    ..Some specific ones to watch:

        Begin Programming A PIC16F1xxx in C like a Pro
        https://www.youtube.com/watch?v=Lm6e3xSC8sg

        Taming Embedded C - Pt 1
        https://www.youtube.com/watch?v=BtXyvVy67Qs

    Q/A: Where can I find microcontroller code examples (PIC16F, etc)
    https://microchip.secure.force.com/microchipknowledge/articles/en_US/FAQ/Where-to-find-code-examples-and-libraries-for-Microchip-microcontrollers-PIC16F-PIC18F-PIC24-dsPIC-etc/?l=en_US&fs=Search&pn=1

    Some specific pages:

        1. Go to http://www.microchip.com/mplab/microchip-libraries-for-applications 
        2. Download the latest version of MLA (Microchip Libraries for Applications). 
        3. The C:\microchip\mla\v2016_11_07\doc folder (or whichever current version of MLA),
           will have various help documents. 

    16F1709 video:
    https://player.vimeo.com/video/204700953

        Some applications are available only in the Legacy MLA (v2013-06-15),
        so it is recommended to install and check also there.

        In order to download Legacy MLA, go to:
        http://www.microchip.com/mplab/microchip-libraries-for-applications
        ..then scroll-down, select "Legacy MLA" tab and choose the package
        specific to your operating system.

        Note: The MLA is moving to MPLAB Code Configurator (MCC) libraries.

        The old Microchip Peripheral Libraries can be downloaded
        from the website, however, the page mentioned above is now
        obsolete. Therefore, no further updates will be made to those
        libraries. We recommend to move on to MCC and use it for all
        your future projects.

        To set up the registers of a Microchip device,
        you can download MPLAB Code Configurator (MCC) from
        http://www.microchip.com/mplab/mplab-code-configurator
        or by MPLAB X IDE -> Tools ->Plugins. This tool can easily set
        up the registers and peripherals, once it is installed. Check
        if your device is supported by MCC before you try to configure it.

        For additional general MCC information visit
        http://microchipdeveloper.com/mcc:overview

        How to use the IPE video:
        https://vimeo.com/307355242?from=outro-embed

      
      Timers
      ======
      Timer in our case is driven by the instruction clock,
      which is 1/4 the xtal oscillator frequency (_XTAL_FREQ),
      which we'll call FOSC4.

      So, the formula for calculating the number of interrupts
      per second for different prescalar values:

        #define _XTAL_FREQ 4000000UL    // 4MHz
        #define FOSC4 (_XTAL_FREQ/4)    // 1MHz

        ints_per_sec = FOSC4 / prescalar / 256;                 // 256: is the 8 bit counter that triggers the
                                                                //      interrupt each time it overflows

      ..where 'prescalar' is based on this table for PS<2:0> values:
        
        PS<2:0> |  Prescalar value
        --------|-----------------
          000   |   /2
          001   |   /4
          010   |   /8
           :         :
          111   |  /256

      So if PS<2:0> is set to 111, the formula ends up being:

         ints_per_sec = FOSC4 / 256 / 256
         ints_per_sec = 1000000 / 256 / 256
         ints_per_sec = 15.26

      The simplified logic diagram for TMR0 if configured to run
      of the internal instruction clock:
         
         Set INTCON     TMR0IE (IE=int enable) to 1 to enable timer0 interrupt.
         Set OPTION_REG TMR0CS (CS=Clock Select?) to 0 to enable FOSC4 to drive ints.
         Set OPTION_REG PSA (PSA=Prescalar) to 1 to enable prescalar.
         Set TMR0SE (SE=Select Edge?) selects rising or falling edge to trigger ints (who cares)

          _______         __________         _______________________________
         |       \       |          \       | 8 bit timer/counter register  \
         | FOSC/4 >------| Prescalar >------| (basically divides by 256)     >------------> Int Trigger
         |_______/       |__________/       |_______________________________/  overflow
                                                 counts 0-ff, then overflows


      See datasheet for INTCON register: (search for intcon)

          GIE | PEIE | TMR0IE | INTE | RBIE | TMR0IF | INTF | RBIF
                         |
                         1 enables the TMR0 int,
                         0 disables the TMR0 interrupt

      Whenever TMR0's 0-ff counter overflows, the TMR0IF (IF=int flag)
      is set, and an interrupt is triggered. Software can inspect the flag
      to see if this interrupt was generated by TMR0 (if not, the flag would
      be 0, indicating the cause was something else)

      Going in or out of sleep mode disables the timer.
      If you want the timer running during sleep mode, you have to re-enable
      timer ints on to-sleep or from-sleep; see data sheet.

      So, the following code is from the video on interrupts:

      // Interrupt handler (ISR == interrupt service request?)
      //
      // Video's calculation:
      //     Timer at 1:256 clock frequency means:
      //     freq_out = freq_clk / (4*prescalar*(256-TMR0)*K)
      //     freq_out = 20MHz / (4*256*(256-0)*K
      //     K = 20,000,000 / (4*256*256) = 76 (approx) ints per sec
      //
      //     For 4MHz xtal, the calculation is:
      //     K = 4,000,000 / (4*256*256) = 15.26
      //             \         \  \   \__ 8 bit 0-ff counter
      //              \         \  \__ prescalar
      //               \         \__ instruction clock is XTAL_FREQ/4
      //                \__ XTAL_FREQ
      //
      int icount = 0;           // our interrupt counter (0-15 == ~1 sec)
      void interrupt ISR(void) {
          if ( INTCONbits.TMR0IF ) {    // see if timer overflow occurred
              INTCONbits.TMROIF = 0;    // if so, clear timer overflow flag
              icount++;
              if ( icount == 76 ) {
                  icount = 0;
                  // DO INTERRUPT STUFF HERE FOR ONCE A SECOND
                  PORTA = ~PORTA;       // flip all bits of portD
              }
          }
      }

      INTCONbits.GIE = 1;    // enable all global interrupts
      INTCONbits.PEIE = 1;   // enable peripheral ints
      INTCONbits.TMR0IE = 1; // enable timer int
      INTCONbits.TMR0IF = 0; // cleartimer overflow flag
      ei();

      // Configure timer
      //    Search datasheet for for option_reg to set the
      //    1) clock source (TOCS:TMR0 clock source, and 2) prescalar (PS2:PS0)
      //
      OPTION_REGbits.T0CS = 0;  // internal instruction cycle clock (CLK0)
      OPTION_REGbits.T0SE = 0;  // edge selection
      OPTION_REGbits.PSA  = 0;  // assign prescalar to timer 0
      OPTION_REGbits.PS0  = 1;  // --
      OPTION_REGbits.PS1  = 1;  //  |-- together selects 1:256
      OPTION_REGbits.PS2  = 1;  // --

---

