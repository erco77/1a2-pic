/***
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
***/

#define MAX_DEBOUNCE 10

// Lamp cadences
const unsigned int lamp_index = 0;	// adjusted by timer interrupt
const bool hold_lamp[] = { 0,1,1,1,1,1,0,1,1,1,1,1 };	// 1 sec overall (~83 msec each)
const bool ring_lamp[] = { 0,0,0,0,0,0,1,1,1,1,1,1 };	// 1 sec overall (~83 msec each)
// Ring cadence
const unsigned int bell_index = 0;	// adjusted by timer interrupt
const bool ring_bell[] = { 1,1,0,0,0,0 };		// 6 sec overall (1 sec each)

// STRUCT FOR LINE STATE
typedef struct {
    // Inputs (debounced)
    bool line_det  = 0; int line_det_ctr;
    bool ring_ret  = 0; int ring_det_ctr;
    bool a_lead    = 0; int a_lead_det_ctr;
    // Outputs
    bool hold_rly  = 0;
    bool ring_rly  = 0;
    bool lamp_xstr = 0;
    // State
    bool hold_state = 0;
} Line;

// Initialize the Line struct to all zeroes
void InitLine(Line line) {
    line->line_det   = 0; line->line_det_ctr = 0;
    line->ring_det   = 0; line->ring_det_ctr = 0;
    line->a_lead     = 0; line->a_lead_ctr   = 0;
    line->hold_rly   = 0;
    line->ring_rly   = 0;
    line->lamp_xstr  = 0;
    line->hold_state = 0;
}

// Handle input transitions for a single line.
//     Reads the 3 inputs: line detect, ring detect, and A lead.
//     Given that, manage the outputs: lamp, hold relay, ring relay.
//
void HandleLine(Line& line) {
    if ( line->line_det ) {
	// Line in use
	if ( line->hold_state ) {
	    // Call On HOLD
	    if ( line->a_lead ) {
		// Started picking up a HOLD call
		line->hold_rly   = 0;
		line->hold_state = 0;
		line->ring_rly   = 0;
		line->lamp_xstr  = hold_lamp[lamp_index % sizeof(hold_lamp)];
	    } else {
		// Still on HOLD
		line->hold_rly   = 1;
		line->hold_state = 0;
		line->ring_rly   = 0;
		line->lamp_xstr  = hold_lamp[lamp_index % sizeof(hold_lamp)];
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
		line->lamp_xstr  = hold_lamp[lamp_index % sizeof(hold_lamp)];
	    }
	}
   } else {
	// Line idle
	//     Besides just being idle, line might be ringing, or has just
	//     transitioned to idle (dropping from hold, or hung up, or a CPC hangup)
	//
	if ( line->ring ) {
	    // Line ringing
	    line->hold_rly   = 0;
	    line->hold_state = 0;
	    line->ring_rly   = ring_bell[lamp_index % sizeof(ring_bell)];
	    line->lamp_xstr  = ring_lamp[lamp_index % sizeof(ring_lamp)];
	} else {
	    // Line idle or just became idle
	    line->hold_rly   = 0;
	    line->hold_state = 0;
	    line->ring_rly   = 0;
	    line->lamp_xstr  = 0;
	}
    }
}

void DebounceInput(bool inp, bool &latch, int &ctr) {
    // Did input change compared to software latch?
    if ( inp != *latch ) {
        // Increment debounce counter to ignore noise.
	// If counter exceeds max, transition to new state.
	if ( (*ctr)++ > MAX_DEBOUNCE ) {
	    *latch = inp;	// transition latch to new state
	    *ctr   = 0;		// reset debounce counter
	}
    } else {
        *ctr = 0;		// reset debounce counter (noise)
    }
}

void main() {
    // State for lines 1 and 2
    Line line1, line2;
    InitLine(line1);
    InitLine(line2);

    while (1) {
        // DEBOUNCE INPUTS
	{
	    // LINE #1 INPUTS
	    DebounceInput(LATAbits.LATA4, &line1->line_det, &line1->line_det_ctr);
	    DebounceInput(LATAbits.LATA5, &line1->ring_det, &line1->ring_det_ctr);
	    DebounceInput(LATAbits.LATA6, &line1->a_lead,   &line1->a_lead_ctr);
	    // LINE #2 INPUTS
	    DebounceInput(LATBbits.LATA4, &line2->line_det, &line2->line_det_ctr);
	    DebounceInput(LATBbits.LATA5, &line2->ring_det, &line2->ring_det_ctr);
	    DebounceInput(LATBbits.LATA6, &line2->a_lead,   &line2->a_lead_ctr);
	}

        // HANDLE LINE TRANSITIONS
	HandleLine(line1);
	HandleLine(line2);

	// APPLY OUTPUTS
	{
	    // LINE #1
	    LATAbits.LATA1 = line1.hold_rly;
	    LATAbits.LATA2 = line1.ring_rly;
	    LATAbits.LATA3 = line1.lamp_xstr;
	    // LINE #2
	    LATBbits.LATB1 = line2.hold_rly;
	    LATBbits.LATB2 = line2.ring_rly;
	    LATBbits.LATB3 = line2.lamp_xstr;
	}
    }
}
