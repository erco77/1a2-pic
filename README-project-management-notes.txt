SPRINT PCB
----------

* File -> Save As:
	1a2-pic  to desktop
	1a2-pic  to usb		-- do this last, so all below save to USB too

* File -> Export -> Gerber Export
	Enable all 7 gerbers:
		1+2) top/bot copper,
		3+4) top/bot silk,
		5+6) top/bot solder maskl
		7) outline
	Set checkbox: "Include frame (board size)"
	..which seems to always clear itself!

	FOR SURFACE MOUNT RESISTORS: You can leave out the "smdmask", as that's if
	you want to generate a metal mask for solder paste.

* File -> Export -> Excellon
	Make sure "Inches" selected

Then I convert these into a .zip that I can then send to the various PCB print
houses for generating the boards. I found PCBway to work well for the boards I've
printed during development, and they have instructions specifically for Sprint 6.0:
https://www.pcbway.com/blog/help_center/How_to_generate_Gerber_from_Sprint_Layout_6_0.html

[OPTIONAL]
  I like saving out .jpg images of the PCB board so I can review them on the web
  from coffee shops and such to look for errors, and without needing the Sprint
  software to view it. To do this:

    * Show top/bot copper, top silk, outline, then:
      File -> Export -> JPG  -- Save as "1a2-pic--all

    * Show top + outline only, then:
      File -> Export -> JPG  -- Save as "1a2-pic--top

  ..and rinse/repeat for "bottom", "top-and-bot", "photo-top" and "photo-bottom".
  I used a Makefile to package these up to the website.

EXPRESSPCB SCHEMATIC: SAVE
--------------------------
I used ExpressSCH (Express PCB's schematic software) to generate the schematics.

   * Save a copy of the schematic to the USB drive

   * Choose each page in the schematic, then choose File -> Export schematic image,
     saving each as 0001, 0002, etc so I can make them available on the website.

I then used a Makefile to convert the BMPs this generates into 8 color pngs
that it then sends to the website.

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

   o Then you wait -- they take maybe 10 minutes or more to load your zip file
     and check for errors, and then adjust your quote estimate if need be. Just
     reload the page until the person responds who checks your board for errors.

   o Once you get the all clear, you can then order the boards.
     Paypal works fine, that way they never see your credit card info.
     I chose express DHL for delivery, and I typically used their 24 hour 
     express service so that I can keep my intertia on the project.

   o While waiting for the boards to be printed, I'd spend those days
     reviewing my parts inventory, ordering any parts from digikey I might need
     before the boards come. I'd also check the board's schematics for errors,
     as each time I did a board run I was typically making changes, and the way
     I work, I typically start with protoboarded circuits, draw those into the
     board layout program, make some rough schematics from that, and re-proto
     the board based on the PCB layout I end up with, to be sure what I drew
     really will work. And then have the boards printed, and refine the schematics.
     Reviewing the schematics while the boards are bring printed also makes sure
     I have the design in my head so that when the boards come back, I'm primed
     to solder the boards up, and know what to check for during testing.

