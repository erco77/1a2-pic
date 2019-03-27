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

* File -> Export -> Excellon
	Make sure "Inches" selected

* Show top/bot copper, top silk, outline, then:
  File -> Export -> JPG  -- Save as "1a2-pic--all

* Show top + outline only, then:
  File -> Export -> JPG  -- Save as "1a2-pic--top

..and rinse/repeat for "bottom", "top-and-bot", "photo-top" and "photo-bottom".


EXPRESSPCB SCHEMATIC: SAVE
--------------------------
   * Save a copy of the schematic to the USB drive

   * Choose each page in the schematic, then choose File -> Export schematic image,
     saving each as 0001, 0002, etc

UPLOAD
------
Save USB files to network drive, e.g. pic-board--rev-e/PCBWay-print-<date>/*

Upload new files to erco/tmp/pic/ dir on website; on the network drive
there should already be a Makefile that includes a 'send' target, so 'make send'
should rsync it to the website.

GERBER SEND TO WEB
------------------
Use 'make clean; make' to recreate the .zip file from gerbers,
and then upload these to PCBWay, WellPCB, Seeed Studios, OSHPark, whatever..

