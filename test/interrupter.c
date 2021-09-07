#include <stdio.h>
#include <unistd.h>

int main() {
    char G_ring_relay = 0;
    char G_ring_flash = 0;
    char G_hold_flash = 0;

    for ( int r=0; r<5; r++ ) {
	for ( int t=0; t<4000; t++ ) {
	    G_ring_relay = ( t < 1000 )       ? 1 : 0;		// ring 1 sec, 3 sec silence
	    G_ring_flash = ((t % 1000) < 500) ? 1 : 0;
	    G_hold_flash = ((t % 500 ) < 400) ? 1 : 0;
	    if ( t % 25 == 0 ) {
	        printf("%04d) %d %d %d     %s    %s    %s\n",
		    t, (int)G_ring_relay, (int)G_ring_flash, (int)G_hold_flash,
		       (G_ring_relay ? "BELL" : "    "), 
		       (G_ring_flash ? "RING" : "    "), 
		       (G_hold_flash ? "HOLD" : "    ") );
		usleep(25000);
	    }
	}
    }
    return 0;
}
