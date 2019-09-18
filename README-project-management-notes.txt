// vim: autoindent tabstop=8 shiftwidth=4 expandtab softtabstop=4

SAVING GERBERS / ORDERING NEW BOARDS
====================================

    I used Sprint Layout 6.0 (Windows only) to layout the printed circuit board for this
    project. It has a nice UI, similar to ExpressPCB's, but has the option of saving
    gerbers/drill files, so that I can use any PCB printing company in the world for more
    cost effective board printing.

    This software is free to download and use to load and create PCB's, but saving is
    disabled unless you purchase the software (~$50 USD).  I think $50 is very reasonable,
    considering how much other PCB software costs. And $50 is way cheaper than the free
    ExpressPCB software ends up being when you pay $380 to have the boards printed,
    instead of more like $100 to have them printed elsewhere.

    SAVING OUT GERBERS/EXCELLON DRILL
    ---------------------------------
    These are necessary for printing up the boards. I use a Makefile to gather them
    up and zip them together for upload to printing services. Steps in Sprint 6.0 are:

    * Load the layout file:
      File -> Open -> 1a2-pic-IRF-rotary-interlink-REV-F1.lay6

    * Export the Gerbers:
      File -> Export -> Gerber Export

      1) Turn on all layers that are not grayed out.
      2) Be sure "solder mask offset" for "Pads" is 5.0 mils

      Change all the input fields marked with "***":

            Layer  (X) C1 Copper Top                                              Options
                   ***                                                              ( ) Mirror
                   (X) C2 Copper Bottom                                             ( ) Punch drill holes
                   ***
                   (X) S1 Silkscreen Top                                          Offset of solder mask
                   ***                                                              (X) Pads:      5.0 mil   <--- *** Change this! (Default is 10)
                   (X) S2 Silkscreen Bottom                                                        ***      
                   ***                                                              (X) SMD-Pads   3.9 mil

                    :                                                               (X) Others     3.9 mil
                     
                   (X) C1 - Solder mask
                   ***
                   (X) C2 - Solder mask
                   ***
                    :

                   (X) O - Outline
                   ***
                   (X) Include frame (board size)      <--- *** Don't miss this one - It keeps turning off by default!
                   ***

	    > Be sure to set checkbox: "Include frame (board size)". This always seems to clear itself
	    > each time you save, so be sure to set it each time!

            > 5 mils seems a better solder mask offset than 10. Prevents nearby traces from becoming
            > soldered to pads, and keeps some mask between all Amphenol connector pads.

	    FOR SURFACE MOUNT RESISTORS: You can leave the "smdmask" turned OFF, as that's only
	    for generating the metal mask used for solder paste. The few surface mount
	    components on these boards can be hand soldered easily.

    * File -> Export -> Excellon
	    Make sure "Inches" selected

    Then I put the resulting files into a .zip using a Makefile that I can
    then shop around to the various PCB print houses. I used a Makefile to
    automatically generate the zip, but it can be done by hand too using
    tools like Winzip, pkzip, or the zip software that comes with linux.

    I found PCBway to work well for the boards I've printed during
    development, and they have instructions specifically for Sprint 6.0:
    https://www.pcbway.com/blog/help_center/How_to_generate_Gerber_from_Sprint_Layout_6_0.html

    [OPTIONAL] SAVING OUT JPG IMAGES
    --------------------------------
      I like saving out .jpg images of the PCB board so I can review them on the web from
      coffee shops and such to look for errors, and without needing the Sprint software
      to view it. To do this:

        I. 1a2-REV-XX-all.jpg
        ---------------------

        1) At bottom left, ensure visible: "C1", "S1", "C2". Other layers "S2" and "O" should be OFF.
        2) At bottom left, ensure active: "C1". This ensures top copper draws over bottom copper.
        3) File -> Export -> JPG
        4) Resolution: 300 dpi (3rd slider position from left)
        5) Save as "1a2-REV-XX-all"

        II. 1a2-REV-XX-top-bot.jpg
        --------------------------
        Assumes above was just done, and settings still set as above:

        1) At bottom left, disable "S1" (so only C1,C2 are enabled for visible, and C1 still "active")
        2) File -> Export -> JPG
        3) Click OK for Resolution
        4) Save as "1a2-REV-XX-top-bot"

        III. 1a2-REV-XX-photo-top.jpg
        -----------------------------
        Assumes above was just done, and settings still set as above:

        1) Click "Photoview" and turn on "Top side C1/S1", "With silkscreen", Board: Green, Soldermask: Gold
        2) File -> Export -> JPG
        3) Click OK for Resolution
        4) Save as "1a2-REV-XX-photo-top"

        IV. 1a2-REV-XX-photo-bot.jpg
        -----------------------------
        Assumes above was just done, and settings still set as above:

        1) Turn on "Bottom side C2/S2"
        2) File -> Export -> JPG
        3) Click OK for Resolution
        4) Save as "1a2-REV-XX-photo-bot"

        V. 1a2-REV-XX-bom.txt
        ---------------------
        1) Options -> Component Panel
        2) For "Show:", enable Number, ID, Value, Package, Comment
        3) Click "Export" button
        4) In dialog:

            Exported Data:          Separator:             Text for Layer side
           -->  (ON)  Number            (off) Comma              Top: Top
           -->  (ON)  ID                (off) Semicolon       Bottom: Bottom
           -->  (ON)  Value         --> (ON)  Tab
                (off) Layer
                (off) Position      X/Y - Position:     Decimals: 2
                (off) Rotation          Unit: Mil         (OFF) Suppress trailing zeros
           -->  (ON)  Package
           -->  (ON)  Comment       Rotation:
                                        (off) Rotation with "R" prefix

                                    Filter
                                        (ON) SMD-Components                            (ON) Top
                                    --> (ON) Throughhole-Components                    (ON) Bottom
                                        (off) Only components with Pick+Place data

           When set correctly, under "Preview:" it should show at least 100 or more parts.. scroll down.

        5) Hit "Export.."
        6) Save as "1a2-REV-XX-bom", type "Textfiles (*.txt)"
        7) Close..

        VI. SEND CHANGES TO WEBSERVER
        -----------------------------
        Go into 'website' directory and run 'make send' as erco on harris.
        This should build the zip, bom -> html, and send that and jpg's to server.
        Adjust the index.html if needed.


PRINTING BOARDS AT PCB HOUSES
-----------------------------
Typically you upload a .zip file of the gerbers and "Excellon Drill" file
to have the boards printed.

Companies like PCBWay, OSHPark, WellPCB, Seeed Studios, etc.

I used PCBWay for several board runs to do my prototypes and final boards,
and the general instructions for using them:

   o Go to the PCBWay.com website

   o Create an account first, if you don't already have one

   o Use their PCB instant quote

   o Fill out their inital form; I used their defaults.
     They have a default minimum hole size of .3mm (.012 inches), so I made
     sure the board's smallest vias were that size and no smaller.
     Pick any colors you want; default of green board and white silk is fine.
     I usually chose a quantity of 5 for prototypes, or 10 when I was pretty sure
     it was a good board.

   o Choose 'calculate' and it calculates your cost approximately.

   o Choose "Add To Cart"

   o Now you can upload the .zip file of gerbers

   o Then you wait -- they take maybe 10 minutes or more to load your zip file and check
     for errors, and then adjust your quote estimate if need be. Just reload the page
     until the person responds who checks your board for errors.

   o Once you get the all clear, you can then order the boards.
     Paypal works fine, that way they never see your credit card info.  I chose express
     DHL for delivery, and I typically used their 24 hour express service so that I can
     keep my intertia on the project.

   o While waiting for the boards to be printed, I'd spend those days reviewing
     my parts inventory, ordering any parts from digikey I might need before the boards
     come. I'd also check the board's schematics for errors, as each time I did a board run
     I was typically making changes, and the way I work, I typically start with protoboarded
     circuits, draw those into the board layout program, make some rough schematics from
     that, and re-proto the board based on the PCB layout I end up with, to be sure what I
     drew really will work. And then have the boards printed, and refine the schematics.
     Reviewing the schematics while the boards are bring printed also makes sure I have
     the design in my head so that when the boards come back, I'm primed to solder the
     boards up, and know what to check for during testing.

MANAGING SCHEMATICS
===================

    I used ExpressSCH 7.0.2, Express PCB's schematic software (Windows only),
    to generate the schematics.
    
    The software is FREE from ExpressPCB's website.

    To generate images, I load the .sch file, and view each sheet, and choose File -> Export
    to save each image:

       * Load the .sch file

       * Choose each page in the schematic, then choose File -> Export schematic image,
	 saving each as 0001, 0002, etc.
	 
       * After that, I run a Makefile that converts the large .BMP images into smaller
         8 color PNGs so I can make them available on the website.

SOLDERING BOARDS
----------------

    There's two ways I solder up the boards, depending on what I'm doing.

	1) The typical case is creating a production board. The board is known to be a
	   working design, so it makes sense to just solder all the components at once.

	2) The odd case is I'm testing a new prototype board, which might be a dud design.
	   So I solder in JUST the components of a small section I want to test first,
	   so I can easily test it without committing to soldering all components. This way
	   if it's a dud board, I don't have to waste time and component inventory on a board
	   that will never be used.

    So here I'll cover just soldering a production board, as that's the typical case.

    PRODUCTION BOARD SOLDERING
    --------------------------

    I like to start with the smallest profile components first, so that when you flip
    the board over to solder the leads, the component can be evenly pressed from the
    other side using a soft cloth or folded papertowels. You want the components 
    pressed flat when you solder them in, or they won't lay evenly, and in the case
    of sockets, will stress the vias when inserting chips.

    So I tend to do them in this order:

    	> Surface mount resistors

	  I touch a small bead of solder on each pad, then position the resistor over
	  the two pads, and touch the soldering iron to the edge of the pad to 'suck'
	  the component onto the pad. This will be at an angle because the other pad
	  is still hard. Then heat the other pad to liquify the solder, then quickly
	  go back to the first pad (while the second is still liquid) to suck the 
	  component down flat.

	> 6 pin chip sockets

	  These are small and fall out easily, so I do them first.

	  I put at least two in the board, and hold them as I flip the board over onto a
	  soft cloth or folded papertowel that is just under those components so that it
	  presses them against the board. I then solder the leads on the solder side.

	  If in doubt about the component laying flat, I touch solder on a corner pin first,
	  let it harden to hold the socket in place, then flip the board over to view the
	  board sideways to see if the socket is resting flat. If it's not, I can touch
	  the iron to that one pin to loosen it, while pressing on the socket to flatten
	  it, making sure not to put a finger on the pin (it'll be hot!). The reason for
	  doing one pin is it's really hard to reseat a crooked component if all the pins
	  have already been soldered; it's hard to heat ALL the pins at once so you can
	  reseat before the solder hardens.. trust me.

	> Do all the larger chip sockets similarly.

	  Some sockets are nice in that they have pins you can bend to hold the sockets
	  flat and in place when you flip the board for soldering. If that's not an option,
	  solder a few at a time, using a cloth or folded napkin underneath to keep pressure
	  on the sockets to press them flat against the board while you solder the pins.

	> Do all the small through hole discrete components (resisitors, capacitors..).

          I like to solder all the resistors with the color bands facing the same way,
          the first band digit facing up or facing left. I try to be consistent, as it
          simply looks nicer that way.

	  For discretes I try to do a bunch at a time, having them all pre-inserted into
	  the holes, pre-bent into a "U" shape with needle nose pliers, and hold them in
	  place with my finger tips while I flip the board, and bend all the leads at 45
	  degree angles to hold them in place:


                                       \                  / 
                                        \                /  <-- bend leads 45 degrees
                    Solder Side          \              / 
	            ------------------------------------------------------------  Board
	            ------------------------------------------------------------
		    Component Side        |__::::::::__|
				             ::::::::

				             Resistor


          Then once a bunch of them are inserted this way, I solder them all in one pass,
          then trim off the leads with small wire cutters.

          NOTES:

              1) Keep a small cup nearby to put all the cutaways into, so they
                 don't end up flying everywhere, or getting under and between chip sockets
                 to cause shorts later.

              2) Keep the soldering area clean!! Don't let cut off leads or solder blobs
                 lie around, or they'll find their way into the board causing shorts later.

        > Solder the TO-220 transistors. 
        
          NOTES: 
              1) TO-220's are best laid flat and screwed down, to prevent them from flapping
                 around, but more importantly, to keep them from sliding sideways into
                 each other, shorting them out, as the heat sink tabs are the output of
                 the transistor.

              2) When making the 90 degree angle bend for the pins, don't bend the pins where
                 they meet the case. Bend them out at the point where the leads flare down
                 from fat to thin. e.g.

                                     ______________
                                    |       _      |
                                    |      (_)     |
                                    |______________|
                                    ||             |
                                    ||             |
                                    ||             |
                                    ||_____________|
                        wide leads --> | | | | | |\___ DON'T bend here
                                       |_| |_| |_| 
                                        |   |   |\___ BEND HERE
                         thin leads --> |   |   |
                                        |   |   |

          I pre-bend the leads with needle nose pliers before inserting them into the
          board. Once inserted, I screw the case down with nuts so they stay in position
          when I flip the board to solder them.

          Some notes on pre-bending the pins: When bending the leads, just bend along the
          thin part of the leads, not at the case:

                          ________                               ________
                 ________|        |                     ________|        |___
                 |_______|________|\   BAD !            |_______|________|   \   GOOD
                                    \                                        |
                                    |                                        |
                                    |                                        |

         To do this, I grip small needle nose across the fat pins:


                            plier's grip                     
                            on fat pins                                            ___________________
                            |                                                     /   ________________\
                           \|/                                                   /   /
                            _                                    _______________/   /
                  ________ / \                                  /________          /
         ________|        |===-------      <--- pins --->       _O_O_O___/         \
         |_______|________|\_/                                  \________________   \
                                                                                 \   \________________
                                                                                  \___________________/
                                                                                                   
                - SIDE VIEW -                                                  - BOTTOM VIEW -
                                                       

         To prevent metal-fatiguing the pins with a full 90 degree turn all at one point
         on the leads, I bend the pins down only 45 degrees with my finger, the needle nose
         prevents bending the wide part of the pins:


                              ::: <-- plier teeth
                     ________ :::
            ________|        |===
            |_______|________|::: \ <-- bend leads 45 degrees downward at pliers
                              :::  \
                                    \
                                     \


        ..then re-grip the pliers around the 45 degree bend, and bend again another 45 degrees:

                     ________
            ________|        |===  /\ <-- plier teeth
            |_______|________|    \\/
                                 /\\
                                 \/ |  <-- bend again at pliers 45 degrees downward
                                    |


        The result ends up being more of a curve than two flat 45 degree bends, giving a
        nice bend radius that can't really be render in ascii art:

                     ________
            ________|        |===
            |_______|________|    \  <-- a nicer looking curve
                                   \     than this ascii art can show
                                    |
                                    |

     > Solder the large profile components last (relays, large caps, connectors, SIP + DIP headers)

     > Last I usually do the press-fit 50 pin amphenol connectors.

       I used a large clamp and a simple wood jig with a slot cut in it. The jig goes under the
       board for the clamp to clamp against, the slot allows the pins to poke through without
       being compressed by the clamp:
                                      __________________
                                     /   ____________   \
                                    /____\           \   \
                                                     |    |  <-- clamp
                                      __             |    |
           press fit amphenol -->    |  |            |    |
           connector (side view)    _|__|_           |    |
                                    |____|           |    |
                                      ||             |    |
    PCB BOARD                                        |    |
    ===============================================  |    |
                            ________      ________   |    |
               wood --->   |        |____| slot   |  |    |
               jig         |______________________|  |    |
                                    _____            |    |
                                    \___/            |    |
                                     ///             |    |
                                    _///_            |    |
                                   |     |__________/     |
                                    \____________________/
                                     ///
                                     ///
                                     ///
                                    /   \===========()
                                    \___/

    I found pressing first at the edges, then the center last works best.
    So in three pressings, I'm able to get the connector pressed into place
    completely. You have to make sure the clamp is CENTERED on the connector,
    both above and beneath, or the connector will tip sideways. Stop and reposition
    as soon as you see that happening, or it'll bend the pins.

    When pressed in properly, the result looks like this:
    
                                      __________________
                                     /   ____________   \
                                    /____\           \   \
                                     |  |            |    |
                                    _|__|_           |    |
                                    |____|           |    |
    ==================================||============ |    |
               wood --->   |        |____| slot   |  |    |
               jig         |______________________|  |    |
                                    \___/            |    |
                                     ///             |    |
                                     ///             |    |
                                    _///_            |    |
                                   |     |__________/     |
                                    \____________________/
                                     ///
                                    /   \===========()
                                    \___/

     Note how the jig's slot is the key to doing this right, to leave clearance
     for the pins when they poke through the board, while still keeping uniform
     pressure on the lower part of the board.

     Once pressed into place, you DO NOT solder the pins. I mean, that's the whole
     point is not having to make 200 solders for the 4 50 pin connectors.

TODO:
    Programming the PIC chips
    Managing source code, git, Makefile
