# vim: autoindent tabstop=8 shiftwidth=4 expandtab softtabstop=4

                    88        db         ad888888b,       88      a8P  ad88888ba  88        88  
                  ,d88       d88b       d8"     "88       88    ,88'  d8"     "8b 88        88  
                888888      d8'`8b              a8P       88  ,88"    Y8,         88        88  
                    88     d8'  `8b          ,d8P"        88,d88'     `Y8aaaaa,   88        88  
                    88    d8YaaaaY8b       a8P"           8888"88,      `"""""8b, 88        88  
                    88   d8""""""""8b    a8P'             88P   Y8b           `8b 88        88  
                    88  d8'        `8b  d8"               88     "88, Y8a     a8P Y8a.    .a8P  
                    88 d8'          `8b 88888888888       88       Y8b "Y88888P"   `"Y8888Y"'   
                                                                                             
                                                                                                              
                  ,ad8888ba,   ,ad8888ba,   888b      88 888888888888 88888888ba    ,ad8888ba,   88           
                 d8"'    `"8b d8"'    `"8b  8888b     88      88      88      "8b  d8"'    `"8b  88           
                d8'          d8'        `8b 88 `8b    88      88      88      ,8P d8'        `8b 88           
                88           88          88 88  `8b   88      88      88aaaaaa8P' 88          88 88           
                88           88          88 88   `8b  88      88      88""""88'   88          88 88           
                Y8,          Y8,        ,8P 88    `8b 88      88      88    `8b   Y8,        ,8P 88           
                 Y8a.    .a8P Y8a.    .a8P  88     `8888      88      88     `8b   Y8a.    .a8P  88           
                  `"Y8888Y"'   `"Y8888Y"'   88      `888      88      88      `8b   `"Y8888Y"'   88888888888  
                                                                                                              
                                                                         
                              ,ad8888ba,        db        88888888ba  88888888ba,    
                             d8"'    `"8b      d88b       88      "8b 88      `"8b   
                            d8'               d8'`8b      88      ,8P 88        `8b  
                            88               d8'  `8b     88aaaaaa8P' 88         88  
                            88              d8YaaaaY8b    88""""88'   88         88  
                            Y8,            d8""""""""8b   88    `8b   88         8P  
                             Y8a.    .a8P d8'        `8b  88     `8b  88      .a8P   
                              `"Y8888Y"' d8'          `8b 88      `8b 88888888Y"'    
                                                                         

                                ==============================================
                                   SERISS CORPORATION - 1A2 KSU CONTROL CARD
                                            2 LINE / 4 EXTENSIONS
                                ==============================================
                                    erco@seriss.com - 04/05/2019 - REV E1

                        If you find problems in this document, contact: erco@seriss.com

                     ____________________________________________________________________
                    |                                                                    |
                    |      o      o      o                o         EXT1 EXT2 EXT3 EXT4  |
                    |     LINE1  CPU    LINE2            ICM         /|   /|   /|   /|   |
                    |     STATUS STATUS STATUS           STATUS      ||   ||   ||   ||   |
                    |                                                ||   ||   ||   ||   |
                    |                                                ||   ||   ||   ||   |
                    |                                                ||   ||   ||   ||   |
                    |                                                ||   ||   ||   ||   |
                    |   ___      __      ____               ____     \|   \|   \|   \|   |
                    |  |   |    |+-|    |oooo|             |oooo|                        |
                    |__|___|____|__|____|____|_____________|____|________________________|
                      Line1+2   12VDC   Stage              Ring   
                      (RJ-11)   Power   Connector          Gen   
                                                           Power  

INITIAL SETUP
=============

There is nothing critical about the order of connecting up the board.
The order shown below is: extensions, power, telco, ring gen.. but
these can actually be connected up in any order. 

0. Mount the board or position it safely before starting
--------------------------------------------------------

    It is important that the board is not lying on anything metal
    when powered it up, to prevent shorting out the board.

    Free Standing
    -------------

    If the board is not mounted to a backboard yet, place the board
    on a non-conducting surface (wood, cardboard, etc) that is free
    of metal parts (no staples, paperclips, stray screws/nuts/wire
    clippings, etc).

    Mounting to a Backboard
    -----------------------

    If mounting the KSU to a backboard, use screws through all
    mounting holes with standoffs to keep the board elevated off
    the backboard's surface so air can flow along the back to aid
    in cooling of components, and to keep it clear of catching stray
    metal (like wire clippings).

                                     .   .
                                  -->.   .<-- 1/4" or more
                                     .   .
                                     .   .
                                     .   XXXXXXX
                                    _    XXXXXXX 
                                   | |___XXXXXXX
                         +------> || |___XXXXXXX
                         |         | |   XXXXXXX
                         |         | |   XXXXXXX
                         |         | |   XXXXXXX
                         |         | |   XXXXXXX
                         |        /| |   XXXXXXX
                         |       / | |   XXXXXXX
                         |    KSU  | |   XXXXXXX
                         |  Board  | |   XXXXXXX
                         |         | |___XXXXXXX
        Mounting Screws -+------> || |___XXXXXXX <-- Backboard
                                   |_| / XXXXXXX
                                  /   /  XXXXXXX
                       PC Board _/   /   XXXXXXX
                                    /
                       Statndoffs _/

                   Figure A: Mounting KSU card to backboard.
                  Standoffs can be metal, nylon, rubber, etc.


        If metal standoffs are used, make sure their diameter does not
        exceed the diameter of the white circles around the mounting holes,
        to prevent shorting traces on the board.

        It's recommended ALL screw mounting holes be used, especially the
        corners and center screw, so that plugging/unplugging the
        large 50 pin connectors doesn't put mechanical bending
        stress on the board that can fatigue the solder/traces.


1. CONNECT POWER
----------------

    Connect power by plugging in the 12volt power:
    
     ____________________________________________________________________
    |                                                                    |
    |      o      o      o                o         EXT1 EXT2 EXT3 EXT4  |
    |     LINE1  CPU    LINE2             ICM        /|   /|   /|   /|   |
    |     STATUS STATUS STATUS            STATUS     ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |   ___      __      ____               ____     \|   \|   \|   \|   |
    |  |   |    |+-|    |oooo|             |oooo|                        |
    |__|___|____|__|____|____|_____________|____|________________________|
      Line1+2   12VDC   Stage              Ring   
      (RJ-11)   Input   Connector          Gen   
                 .                         Power  
                /|\
                 | 
           ......................................................
           : Attach 12VDC power here. +12VDC on LEFT terminal,  :   
           : negative on RIGHT terminal. Must be 850mA minimum  :
           : or higher. Stock power blocks are 2000mA (2 amp).  :
           :....................................................:

    With 12V power connected, the red CPU STATUS light should start
    flashing, 1 blink per second.  This indicates the CPU is operating normally. 
    
    If this LED is NOT flashing, something is WRONG - disconnect power immediately,
    and refer to the TROUBLESHOOTING section at the end of this document.

    IMPORTANT: DON'T CONNECT 12V POWER WITH POLARITY REVERSED.
               This board has NO PROECTION against reverse polarity DC power;
               doing so may blow random components on the board.

    It is advised to use the 12VDC power supply that comes with the board
    which is rated 12VDC / 2 Amps. If you use your own DC power supply source,
    input voltage can safely range from 11.25VDC to 12.75VDC without ill effects.
    1 amp is the suggested minimum rating, more if you have more than 4 extensions.


2. CONNECT 1A2 PHONE EXTENSIONS
-------------------------------

    Connect the phone extensions to the card by connecting the large
    amphenol connectors.

     ____________________________________________________________________
    |                                                                    |
    |                                               EXT1 EXT2 EXT3 EXT4  |
    |                                                /|   /|   /|   /|   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |   ___      __     ____               ____      \|   \|   \|   \|   |
    |  |   |    |+-|   |oooo|             |oooo|                         |
    |__|___|____|__|___|____|_____________|____|_________________________|
      Line1+2   12VDC   Stage              EXT       .     .    .    .
      (RJ-11)   1.5A    Connector          RING     /|\   /|\  /|\  /|\
                                           GEN       |     |    |    |
                                               ....................................
                                               : Attach extensions to the         :
                                               : large 50 pin Amphenol connectors :
                                               :..................................:

    IMPORTANT: When connecting phones for the first time, VERIFY the phones
    are internally wired as follows BEFORE connecting them:
    
            > Bells on pins 20/45 (Y-S pair)
            > Buzzers on pins 17/42 (Y-O pair)
            > Phone networks configured for diode ringing (**)

    (**) Since this board uses a diode network to program ringing, bell
         ringing is on only 1/2 of the AC wave. Paraphrasing Ed from 
         Sundance Communications:

            "For any kind of diode ringing to work, the capacitor between
             network terminals A and K (inside phone) have to be bypassed.
             Move the wire on terminal 'K' to terminal 'A' along with the
             existing wire." The wire he's referring to is usually the actual
             bell's red wire.

       The wiring change inside the phone should be as follows:

   ...................................................................................
   :                                       :                                         :
   :                 BEFORE                :                  AFTER                  :
   :                 ------                :                  -----                  :
   :       ____                            :        ____                             :
   :      /    \                           :       /    \                            :
   :     |  .   |                          :      |  .   |                           :
   :     |      |                          :      |      |                           :
   :      \____/                           :       \____/                            :
   :     __|__|__  RED            _        :      __|__|__  RED              _       :
   :    |        |-------------->(_) "K"   :     |        |--------.        (_) "K"  :
   :    |        |                _        :     |        |         \        _       :
   :    |  BELL  | BLK           (_) "A"   :     |  BELL  | BLK      `----->(_) "A"  :
   :    |        |-----+          |        :     |        |-----+            |       :
   :    |________|     |          |        :     |________|     |            |       :
   :                   |          |        :                    |            |       :
   :                   |          |        :                    |            |       :
   :                   O          O        :                    O            O       :
   :                   /         /         :                   /            /        :
   :                Y/S       S/Y          :                Y/S          S/Y         :
   :                (45)      (20)         :                (45)         (20)        :
   :                                       :                                         :
   :              PHONE TERMINAL BLOCK     :                PHONE TERMINAL BLOCK     :
   :                                       :                                         :
   :.......................................:.........................................:

           Figure B. The diode ringing in-phone wiring change needed for bells.


3. ATTACH PHONE LINES
---------------------

    Connect the card to your telephone service.
    The phone source can be VoIP, XLink BT, or other "plain old telephone" compatible source.

     ____________________________________________________________________
    |                                                                    |
    |                                               EXT1 EXT2 EXT3 EXT4  |
    |                                                /|   /|   /|   /|   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |   ___      __      ____               ____     \|   \|   \|   \|   |
    |  |   |    |+-|    |oooo|             |oooo|                        |
    |__|___|____|__|____|____|_____________|____|________________________|
      Line1+2   12VDC   Stage              Ring   
      (RJ-11)   Input   Connector          Gen   
         .                                 Power  
        /|\          
         |             
         Attach to VoIP, XLink, etc.


    NOTE: Xtreme Technologies "Xlink BT" and "XLink BTTN" are Bluetooth
          Gateway devices that can interface cell phones to PTSN output
          over BlueTooth. It works fine to use that device as the PTSN source,
          so that your 1A2 phones can work with your cellphone, both for
          receiving and originating calls.

    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
    WARNING: This card is NOT approved by the FCC for use directly with
             analog PSTN/POTS lines ("Public Switched Telephone Networks",
             "Plain Old Telephone Service")

             Legally it can only be attached to phone systems that
             use an interface of some kind, like VoIP or like the
             XLink described above.

             While this board IS designed to handle old analog POTS lines,
             THE RESPONSIBILITY IS YOURS whether to connect it without 
             FCC approval. If you do, it is assumed you are a hobbiest
             or EE technician who understands the implications.

    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


4. ATTACHING A RING GENERATOR (IF YOU HAVE ONE)
-----------------------------------------------

    This step is optional. SKIP THIS STEP, if:

        > You don't have an external ring generator

        > You intend to use "BUZZ RINGING" instead, or you have your own
          external ringer and don't want the 1A2 phones themselves to ring.

    To configure a ring generator with the board, you can attach a 105VAC 20Hz-30Hz
    ring generator to enable bell ringing in the 1A2 phones on the Y/S wire pair.

    There are at least three ways to connect an external ring generator,
    depending on the type:
    
      4.1 For an old telco ring generator (like the AT&T 118A), you can wire
          the two Ring AC wires to the RIGHT HAND TWO TERMINALS
          (labelled 105 VAC) on the 4 pin EXT RING GEN connector:


                              EXT RING GEN
                                      
                              (+) (-) (~) (~)
           _ _ _____________________________________________________ _ _
                             | O   O   O   O |
                             |_______________|
                               x   x   .   .
                                      /|\ /|\
                                       |   |
                                       |   |
                                       |   |
                                    ------------     AC POWER
                                   |  118A      |      ____
                                   |  RING      |     |    |__
                                   |  GENERATOR |-----|    |__
                                   |____________|     |____|


      4.2 For the PowerDSINE 12VDC ring generator, this board provides
          a 6 pin header at JP3 for directly connecting this device.

          The SIP header is located to the right of the "EXT RING
          GEN" connector, under the large EXT1 connector, and is
          labled "POWERDSINE RING GEN" and "JP3":

     ____________________________________________________________________
    |                                                                    |
    |                                               EXT1 EXT2 EXT3 EXT4  |
    |                                                /|   /|   /|   /|   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                ||   ||   ||   ||   |
    |                                                \|   \|   \|   \|   |
    |   ___      __     ____               ____                          |
    |  |   |    |+-|   |oooo|             |oooo|   oooooo                |
    |__|___|____|__|___|____|_____________|____|_________________________|
      Line1+2   12VDC   Stage              Ring      .
      (RJ-11)   Input   Connector          Gen      /|\
                                           Power     |
                                                     |
                                              Location of "JP3"
                                                  


          The PowerDSINE's 6 pin connector plug should be inserted into 
          the JP3 header as follows, note the YELLOW wire is to the LEFT:


                          POWERDSINE RING GEN
                           ______________________________
                          |  _    _    _    _    _    _  |
                          | (_)  (_)  (_)  (_)  (_)  (_) |
                      JP3 |__|____x____|____|____|____|__|
                             |         |    |    |    |
                             |         |    |    |    |
                            YEL       BLK  RED  WHT  BLU
                             |         |    |    |    |
                             |         |    |    |    |
                           __|_________|____|____|____|__
                          |                              |
                          |                              |
                          |                              |
                          |                              |
                          |          PowerDSINE          |
                          |       PCR-SIN03V12F20-C      |
                          |         (or similar)         |
                          |                              |
                          |                              |
                          |                              |
                          |______________________________|

              Figure C. Wiring the PowerDSINE via 6 pin plug to KSU board.
                        Avoid connecting the plug in reverse, as the plug
                        does not prevent one from doing so.


      4.3 If you have a PowerDSINE ring generator with the connector cut off,
          or if you have a Cambridge Electronics 12V ring generator, you can wire
          it directly to the EXT RING GEN screw terminals as follows, 
          leaving JP3 unconnected:


                                  EXT RING GEN
                                  (+) (-) (~) (~)
               _ _ ______________________________________________ _ _
                                 | O   O   O   O |
                                 |_______________|
                                   |   |   |   |
                                   |   |   |   |
                                  RED BLK WHT BLU
                                   |   |   |   |
                                   |   |   |   |
                              _____|___|___|___|_____
                             |                       |
                             |                    YEL|------ (tape off, do not connect)
                             |                       |
                             |       PowerDSINE      |
                             |   PCR-SIN03V12F20-C   |
                             |                       |
                             |                       |
                             |_______________________|

                Figure D. Wiring the PowerDSINE to screw terminals.


         For the PowerDSINE, RED/BLK wires are +12/GND outputs to power the device
         from the board, and WHT/BLU wires are AC ring outputs from the device
         to the board. Leave the YEL wire ("inhibit") unconnected.

         For the Cambridge Electronics device, wire the +12/GND inputs to the
         left hand screws on the EXT RING GEN connector, and wire the AC output
         to the right hand screws.

         If you have trouble getting one or more extensions to ring, check the
         TROUBLESHOOTING section below.


5. SET THE DIP SWITCHES FOR RINGING
-----------------------------------

    There are two sets of 8 dip switches at the bottom of the board,
    SW1 (left) and SW2 (right).

        BUZZ RINGING -- SW1
        -------------------
        If you don't have a ring generator, this board can still "ring"
        the phones using the phone's built-in buzzers. Assuming your
        buzzers are wired to the 17/42 wire pair in each phone (Yel/Orn pair),
        TURN ON the appropriate DIP switches under the "BUZZ CALL" section
        (SW1). The 12VDC supply will handle buzzing the buzzers during ringing.

        BELL RINGING -- SW2
        -------------------
        If you have a ring generator attached (step #4) and want the 
        phone extensions to ring, turn on the appropriate DIP switches
        under the "BELL CALL" section (SW2).

    If you don't want the phones to buzz or ring during incoming calls,
    such as if you have your own ringing system (bell boxes, etc), then
    you can turn off /all/ the DIP switches for SW1 and SW2, and leave
    any ring generator connection unconnected.

    Note that for each switch turned on, that enables ringing for a particular
    line to a particular extension. The first two switches manage Ext 1,
    the second two switches manage Ext #2, etc. Here's a full breakdown:

                  Switch #1: ON for ringing Line #1 on Extension #1
                  Switch #2: ON for ringing Line #2 on Extension #1

                  Switch #1: ON for ringing Line #1 on Extension #2
                  Switch #2: ON for ringing Line #2 on Extension #2

                  Switch #1: ON for ringing Line #1 on Extension #3
                  Switch #2: ON for ringing Line #2 on Extension #3

                  Switch #1: ON for ringing Line #1 on Extension #4
                  Switch #2: ON for ringing Line #2 on Extension #4

    This switch arrangement is the same for both SW1 (Buzz Call)
    and SW2 (Bell Call).


TESTING YOUR SETUP FOR THE FIRST TIME
=====================================

    With the above "Setup" steps completed and power applied, you should
    now be able to test the phones. Try these tests, in order:

    Intercom Test
    -------------

         o Pick up the handset on a phone extension, and press the
           RIGHT-most line button, to pick Line #5. This is the
           intercom line.

         o The lamp for that line should light on all the extensions.

         o Try dialing "1" on the touchtone pad. It should buzz the phone
           plugged into EXT1 on the card. Dial "2" to buzz the EXT2 phone,
           etc.

         o While you're using the intercom line, have someone else pickup
           the same line on a different extension. You should both be able
           to talk to each other clearly over the intercom line.

    Call Test
    ---------

         o Try calling Line #1's phone number from a cell phone, or another line.
           The ringers should ring (if configured), and the Line #1 lamp should
           flash at a 1 second rate.

         o Answer the call by selecting Line #1 on the phone, and picking up the
           handset. The lamp should stop blinking and be on steady.

         o Try putting the call on hold by pressing the red HOLD button, and
           hang up. The Line #1 lamp should flash at a fast rate, and the call
           should remain on hold, until you pick up the call again from any
           of the extensions.

         If you don't have a live phone network configured, you can force ringing
         using the "Stage Connector" by shorting the two left terminals together
         to ring line #1, or short the right two terminals to ring line #2:

                      Stage Connector
                        Line 1  Line 2
                         Ring    Ring
                        _____   _____
                       /     \ /     \
   _ _ _____________________________________________________ _ _
                     |  O   O   O   O |
                     |________________|
                        .   .
                       /|\ /|\
                        |   |   <-- jumper or doorbell switch
                        |___|

         To ring Line #1, connect left two terminals on Stage Connector
         together briefly with a wire jumper or doorbell type switch.
         To ring Line #2, do the same for the right two terminals.

         This is useful for demos, where you want to demonstrate ringing
         with just power, but no phone line.

    If you have trouble getting ringing to work, see the TROUBLESHOOTING
    section below entitled "No Ringing".

USING YOUR 1A2 MULTILINE PHONE SYSTEM
-------------------------------------

    Each multiline phone has "line select" buttons to let you
    pick which phone line you can use.

    HOLD
    ----
    From left-to-right, the first button is the red HOLD button,
    which lets you put calls on Hold.

    LINE BUTTONS 1-5
    ----------------
    Next to the red button are the line buttons; the first being
    Line #1, Line #2, and so on, the last line being Line #5, 
    which is the Intercom Line on this board, described below.

    Each phone also contains a bell ringer and a buzzer.
    Depending on how your system is set up, typically the
    phones "ring" when there's an incoming call, and "buzz"
    when someone wants you to pickup the intercom line to talk.
    (See "USING THE INTERCOM" below)

    TO MAKE A CALL
    --------------
    To make a call, push an available line button (that is not already
    lit). Choose the line button BEFORE picking up the handset, so you
    don't accidentally pick up a used line.

    Now lift the handset. The line's button should light up on all
    extenions, indicating you're using the line. You should hear dial tone.

    Dial your call normally, and hang up when you're done.
    When you hang up, the line's light will go out on all phones,
    indicating the line is available for new calls.

    PUTTING CALLS ON HOLD
    ---------------------
    You can put an active call on hold by pressing and releasing the
    red HOLD button. 
    
    Once on hold, the line's lamp should "wink" at a fast rate, indicating
    the call is on hold. You can then hang up the phone, and the call will
    remain on hold.

    Any extension can then pick up the winking line to return from hold
    and continue the call.

    If the remote caller hangs up while they are on hold, the light will
    go out, hold will be released, and the line will be free for use again.

    INCOMING CALLS
    ---------------
    When a call comes in, the extensions programmed to ring should ring,
    and the lamp for that line should flash slowly on all extensions,
    indicating which line is ringing.
    
    Phone extensions should either ring or buzz, depending on how
    the phones are programmed to ring. (See RING PROGRAMMING under "Setup").

    USING THE INTERCOM
    ------------------
    The intercom is a way for people in the office to "inter-communicate",
    such as if a receptionist wants to notify someone at another extension
    there's a call for them, or if one person wants to briefly talk to
    another.

    To use the intercom, push the Line #5 button, and dial the number
    of the extension you want to talk to. Pushing "1" on the dial pad
    will buzz the buzzer for that extension. If a person is available
    at that extension, they should pick up the intercom line and talk
    to you.

    If your phone buzzes, and the intercom line is lit, it means
    someone wants to talk to you, so pick up that line to talk to
    them.

    Since phones buzz whenever you push the extension's dial button,
    people can pre-arrange "codes" for buzzing, for instance two
    buzzes means the call on hold is for you, three buzzes means
    someone is here to see you, etc.


PHONE SYSTEM ACCESSORIES
------------------------

    ORDERING PHONES
    ---------------
    To order more phones, check Ebay, Amazon, or other surplus
    sources for 1a2 multiline phones. AT&T 1A2 models 565, 2564, 
    2565, and 2830 all have been tested to work with this board.
    Other models may work as well; check the Bell System Practices
    (BSP) wiring diagrams to make sure.

    ORDERING EXTENSION CABLES
    -------------------------
    Extension cables for 1a2 phones are available in a large
    variety of lengths, from short 5 foot extensions to long
    100 foot cables or more.

    Practically, it's best to purchase several 25' cables than
    one long cable, as they can be chained together easily if
    need be to attain longer lengths for flexibility.

    Extension cables need male connectors on one end, and female
    on the other; male end connects to the control card, female
    to the phone's own cable.
    
    These cables tend not to be cheap, as they have many conductors
    and the connectors are industrial quality.  Prices vary between
    $1.00 and $2.00 per foot.. so a 25' cable might be as much as $40.
    (2018 prices).

    There are many sources for these cables, used or new, including
    ebay and Amazon. Search terminology for this cables vary:
    
         o RJ21 telco cable
         o Amphenol telco cable
         o Cat3 25 pair telco cable
         o Cat3 50 conductor telco cable
         o 26AWG UTP Telco 25PR

    You can use punch blocks with amphenol connectors to punch down cable
    wiring yourself. The cable can be multiple runs of 8 conductor Cat3, Cat5,
    or other telecom compatible UTP (twisted pair) cable.

    Cable terminology: 
    
        "RJ21" refers to the 50 pin amphenol connectors.

        "Cat3" refers to the twisted pair rating for voice communications,
        where the pairs of wire are twisted around each other in a specific
        way to prevent electrical noise from getting into the voice circuit
        over long runs of wire. I believe higher category numbers, such as
        Cat5, Cat6, etc. should also be OK for old 1a2 wiring, but check
        around for verification.

        "UTP" stands for "Unshielded Twisted Pair", which is typical of
        telephone cables for noise reduction.

        "25PR" means "25 pairs" of wire inside the cable, or 50 conductors total.

        "AMP" or "Amphenol" refers to the connector ends.

        M/F, F/M, F/F, M/M refer to the connector types at either end of the cable.
        You only want M/F or F/M (they're the same thing):

            o M/F or F/M refer to Male/Female connector ends
            o F/F refers to Female/Female connector ends (you DON'T want these)
            o M/M refers to Male/Male connector ends (you DON'T want these)

    Be sure to get Male/Female ("M/F" or "F/M") connector ends on your
    extension cables.

    Conductors can be solid or stranded; typically stranded is more
    flexible, as solid core wire is "stiff".

    RING GENERATORS
    ---------------
    An external ring generator is required to offer the option of
    ringing the bells in the 1a2 phones.
    
    Note bell ringing is not required: phones can still be "rung"
    using the "BUZZ CALL" option, which if configured will "ring"
    the phones using the 1a2 phone's built in buzzers.

    Since this board uses a 12VDC supply, the "Power DSine"
    PCR-SIN03V12F20-C 12 volt ring generator works well. While
    no longer manufactured, it's still only about $20.00 on
    Amazon and Ebay.
    
    During ringing of 4 lines, it seems to use about 500mA (0.5A),
    so make sure your 12vdc power supply can supply at least 1.5A,
    as the rest of the card uses less than 1 amp under full load of
    4 extensions + 2 lines.

    Another manufacturer is Cambridge Electronics, which sells
    a similar 12volt ring generator called the "Black Magic".

    Or, you can use any of the old telco ring generators
    that provide ~70-105 VAC, 20-30Hz ring voltage like the
    118A.


    ENLARGING BEYOND 2 LINES/4 EXTENSIONS
    -------------------------------------
    One can add even more extensions and lines than the two lines/
    4 extensions the card supports out of the box.
    
    A person knowledgable about wiring phones and networks can break
    out any one of the 50 pin extension cables on a 66 block to expand
    support for several extra phones.

    And two cards can be combined with a 66 block to support up to
    4 incoming lines total.

    When attaching extra extensions, beware of the power limits of your
    power supply, and the output limits of the TIP32 power transistors
    Q1 and Q2.  The lamps will glow dimmer when too many lamps are
    lit at once, and/or components Q1 and Q2 will get warm or hot.

    Note that the "LAMP" power transistors have been burn tested to
    handle up to 8 lamps for each line, so up to 8 extensions should
    work OK.
    
    Beyond that, you're on your own. There are apparently "Lamp Expanders"
    (such as the Melco KT-631) which may be able to amplify the Line #1
    and Line #2 lamps for more extensions.

    The ring generator can only handle ringing as many phones as
    its REN number is rated for.

    ADDING OPTIONAL EXTENSIONS #5-#8
    --------------------------------
    There are only 4 extension connectors on the board, but the intercom
    DTMF detect circuit can handle up to 8 extensions max.
    
    The extra 4 extensions (5-8) can be used by expanding EXT4 to a
    66 block, and distribute pins 47-50 to the extra extensions.
    See the board schematic page entitled "Wiring EXT5-8" for how to do this:
    http://seriss.com/people/erco/tmp/pic/data/1a2-pic-REV-E1-0012.bmp

TROUBLESHOOTING
---------------

 This section should help you solve the various problems you can have
 during setup, or when things go wrong. 

 Email erco@seriss.com if you encounter problems not defined here, esp.
 if you've found a solution and think others would benefit from it.
 

 *  PROBLEM: Can't dial with Touch-Tone.
    ------------------------------------
    
    A common problem is TouchTone dialing won't work to dial out on Line #1
    or Line #2; pushing buttons generates no tones, or very low volume tones,
    but dialing on the intercom works fine.

    Things to check:

         > Check the telco line polarity.
         
           Many of the old Touch-Tone dial pads are sensitive to line polarity,
           so be sure it's right. If necessary, flop the wires at your jack.

           Beware some female/female RJ-11 adaptors and/or RJ-11 cables
           are wired to swap Tip/Ring. So even though the jack may be
           wired correctly, the wire or adapters between the jack and KSU
           might be causing the swap problem.

           If Tip/Ring are swapped, these TouchTone pads either won't
           dial at all, or will put out very low volume tones.


 *  PROBLEM: Line #1 works, but Line #2 does not
    --------------------------------------------

    You've confirmed your jack has an active Line #1 and Line #2, but
    Line #2 doesn't work when connected to the KSU.

    Things to check:

        > Check the telephone cable you have connected between the jack
          and the KSU, and verify it's a 4 conductor wire, and not just 2.
          
          Some phone cables only have 2 conductors, the red/grn pair (line #1),
          and omit the yel/blk pair (line #2), which would cause this problem.

          Closely look at the RJ-11 connectors on your cable; if you only see 
          red/grn and don't see yel/blk, replace the cable with a 4 conductor
          cable instead.

 *  PROBLEM: Picking up a line lights lamps but gives no dialtone
    -------------------------------------------------------------

    Things to check:

         Make sure the 50 pin connectors between the KSU board and
         the extensions are fully seated, and not partially connected.
         It's often easy to have one side of the connector or the other
         popped up, not making a full connection.


 *  PROBLEM: Lamps don't light when picking up a line
    -------------------------------------------------

    Things to check:

         > Verify the telephone line connection.
           If there's no voltage on the telco connection, the line sensing
           won't activate and thus the lamps won't light.

         > Make sure the 50 pin connectors between the KSU board and
           the extensions are fully seated, and not partially connected.

         > Verify the CPU STATUS light is blinking. If it's not, then:

             a) Pickup the intercom line on an extension and leave the line offhook.
                (This will cause the on board capacitor to drain throught he intercom).

             b) Pull power from the board and wait 10 seconds before plugging
                the board back in again.

             c) The CPU STATUS light should be blinking. If it's not, the board
                may need servicing.

 *  PROBLEM: No ringing.
    --------------------

        One or all extensions aren't ringing with their bells during
        incoming calls, or when using the STAGE CONNECTOR (as described
        in the "CALL TEST" section above).

    Things to check:

         > Verify bells of EACH 1A2 extension phone are attached to
           the Yellow-Slate wire pair (pins 20/45).

         > Make sure the 1A2 connectors are all pressed down and seated fully.
           A half-seated connector can cause lack of ringing.

         > Make sure the "BELL CALL" DIP switches (SW2) are set to "on"
          for the lines/extensions you want to ring during incoming calls.

         > Make sure the external ring generator is attached correctly
           and verify AC ring voltage is present during the ring cycle.

           An external ring generator must be attached to the card for
           the extensions to ring by their bells.

           If a POWERDSINE is attached to JP3, verify it's not partially
           seated on the pins, and is not plugged in reverse. When 
           positioned correctly, the BLUE + WHITE wires will be to the RIGHT, 
           and the YELLOW wire to the LEFT:

                             POWERDSINE RING GEN
                             _    _    _    _    _    _
                            (_)  (_)  (_)  (_)  (_)  (_)
                       JP3   |    x    |    |    |    |
                            YEL       BLK  RED  WHT  BLU

           Refer to "Figure C" and "Figure D" near the top of this document
           for more info.

         > For bell ringing to work properly with the diode oriented
           circuit of this board, verify all the 1A2 phones have
           the following one wire modification.  Paraphrasing Ed
           from Sundance Communications:

            "For any kind of diode ringing to work, the capacitor between
             network terminals A and K (inside phone) have to be bypassed.
             Move the wire on terminal 'K' to terminal 'A' along with the
             existing wire." 
             
           The wire he's referring to is usually the actual bell's red wire.
           Refer to "Figure B" (near the top of this document) for more info.

         > 1A2 phones typically have ringers with a single bell + clapper.
           These ringers are sensitive to polarity when mixed with a diode
           ring circuit. If the ringer is not ringing or barely ringing/humming,
           try switching the two wires inside the phone between the bell
           and the hybrid network to see if the bell's polarity is the problem.

         > Note that the 12V output on the "RING GEN POWER" connector only
           outputs 12V during actual ringing. This keeps the power off for
           the ring generator when the phones aren't ringing.
           
           To test ringing without a PTSN network, see "CALL TEST" above,
           using the STAGE CONNECTOR to easily trigger ringing.

 *  PROBLEM: Buzz-ringing not working.
    ----------------------------------

        This card features "buzz ringing", where the buzzers buzz in place
        of bell ringing during incoming calls. This is useful if you don't
        have a ring generator.

    Things to check:

        > Make sure the buzzers are all wired to pins 17/42 (Y-O pair).

        > Make sure the 1A2 connectors are all pressed down and seated fully.
          A half-seated connector can prevent the buzzers from working, while
          other features work fine.

        > Make sure the "BUZZ CALL" DIP switches (SW1) are set to "on"
          for the lines/extensions you want to buzz during incoming calls.


 *  PROBLEM: CPU LED NOT FLASHING

    Things to check:

        > Verify the power supply voltage being fed to the board:
          Put a meter across the two "12V INPUT" terminal screws 
          and verify +12 volts DC is being fed to the board, 
          positive on the left screw, negative on the right. 
          Voltage can range 11.5V to 12.75v, but closer to 12 is best.

        > Ensure the power is true DC, and not "lumpy DC"; put a scope
          on the terminal screws to verify DC and not AC voltage.

        > If 12V is coming into the board, check the 7805 (Q3) near
          the power connector, and verify 5V is between pins 2 and 3:

                                  7805
                               -----------
                              |    ( )    |
                              |-----------|
                              |           |
                              |  7 8 0 5  |
                              |           |
                              |           |
                              |___________|
                                 |  |  |     Q3
                                 |  |  |
                                 |  |  |
                                 1  2  3

                           1: +12VDC input
                           2: Ground
                           3: +5VDC output
                                    
          Using a volt meter, keep the black (negative) lead on pin 2
          during the following: touch the red lead to pin 1 should show +12VDC.
          Moving the red lead to pin 3 should show +5VDC.

        > Check the temperature of Q3 (the 7805 at the lower left of the board,
          near power connector). If it gets warm or hot quickly after power is
          connected, SOMETHING MIGHT BE SHORTED OUT - REMOVE POWER QUICKLY.
          
          First let it cool, then unplug all connectors, and then plug in JUST
          the power connector. If that corrects the problem, plug things back
          in one at a time until the culprit is determined.

          Also: check for any stray metal parts touching either the back of
          the board (stray screws, metal dust), or metal bits lying across 
          any of the components on the board that might cause a short. 
          Keep in mind stray metal/liquids lying across components may have
          permanently damaged the board to the point where components are blown,
          and may need to be swapped out to properly repair the board.


* * * EDITING: WORK IN PROGRESS * * *
