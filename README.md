# Happy Birthday Dad

Your very own bowtie! (just a little bit of assembly required)

PINS

Detail Button - D2
State Button - D3
Encoder non-ground pins - D6, D7 (which is which does not matter) (note: the encoder ground pin is the one that is on the right when looking at the non-metal side of the encoder from above with the rounded part facing up)
Display SDA - D5
Display SCL - D1

Instructions

After printing the pieces, put all the components into place. The buttons should slide
into their slots. You'll need to take a small piece of 1/16 x 1/16 wood (balsa works) and push it through the hole in the encoder. Then, align the encoder such that the stick goes through the hole in the side of the encoder opposite the buttons. The crown will then attach on the other side of the bowtie.

As for wiring, the wiring on the bowtie should go up so it can slide over your collar so it can be held in place. Consolidating the ground wires near the bowtie is probably a good idea. The components should be attached as prescribed in the PINS section. Both buttons are normally open, and the other lead should go to ground, such that the pin connected to the button is pulled to ground when they are pressed. You'll also need a wire for the +3.3V, but that only needs to connect to the 5V on the display. (Yes, connecting the 3.3V to the 5V will work just fine as long as you aren't shorting anything. It will provide enough power.)

To power the whole thing, connect two AAA's in series and attach the ground to the GND pin and the positive to the 3V3 pin. How you decide to connect the AAA's is up to you.