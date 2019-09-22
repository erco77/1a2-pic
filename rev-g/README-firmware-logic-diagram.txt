// vim: autoindent tabstop=8 shiftwidth=4 expandtab softtabstop=4

   125 Hz Main Loop
   +---------------------------------+
   |                                 |
   |                                 | A: Line Sense
   |                                / \
   |                               /   \
   |                              /     \
   |                             /       \ yes
   |                            /  Line   \_______________________________________
   |                            \ Detect? /                                       |
   |                             \       /                                        |
   |                              \     /                                         |
   |                               \   /                                          | B: Line in use
   |                                \ / no                                       / \
   |                                 |                                          /   \
   |                                 |                                         /     \ yes
   |                                 |                                        / Hold? \_________________
   |                                 | I: Idle|Ring                           \       /                 |
   |                                / \                                        \     /                  |
   |                               /   \                                        \   /                   |
   |                              /     \                                        \ / no                 |
   |                             /       \ yes                                    |                     |
   |                            / A Lead? \_________________                      |                     |
   |                            \         /                 |                     | F: Active Call      | C: Currently on HOLD   
   |                             \       /                  |                    / \                   / \                       
   |                              \     /                   |                   /   \                 /   \                      
   |                               \   /                    |                  /     \               /     \                     
   |                                \ / no                  |             yes /       \             /       \ yes                
   |                                 |                      |  ______________/ A Lead? \           / A Lead? \________________   
   |                                 |                      | /              \         /           \         /                |  
   |                                 |                      |/                \       /             \       /                 |  
   |                                 | I: Idle|Ring         |                  \     /               \     /                  |  
   |                                / \                     |                   \   /                 \   / no                |  
   |                               /   \                    |                    \ / no                \ /                    |  
   |                              /     \                   |                     |                     |                     |  
   |                         yes /       \                  |                     |                     |                     |  
   |            ________________/  Ring?  \                 |                     |                     |                     |
   |           |                \         /                 |                     |                     |                     |
   |           |                 \       /                  |                     |                     |                     |
   |           |                  \     /                   |                     |                     |                     |
   |           |                   \   /                    |                     |                     |                     |
   |           |                    \ / no                  |                     |                     |                     |
   |           |                     |                      |                     |                     |                     |
   |           |                     |                      |                     |                     |                     |
   |           | J: Ringing          | K: Idle              | G: On Call          | H: HOLD|Hangup      | E: On HOLD          | D: Pickup from HOLD
   |          \|/                   \|/                    \|/                   \|/                   \|/                   \|/
   |      Line Ringing           Line Idle              Line In Use       HOLD after 1/20sec        Call On HOLD        Pickup From HOLD   __
   |   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+     |
   |   |  0 -> LX_hold   |   |  0 -> LX_hold   |   |  0 -> LX_hold   |   |  1 -> LX_hold   |   |  0 -> LX_hold   |   |  0 -> LX_hold   |     |-- L1/L2_hold
   |   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   __|
   |           |                     |                      |                     |                     |                     |
   |          \|/                   \|/                    \|/                   \|/                   \|/                   \|/           __
   |   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+     |
   |   |  0 -> HOLD_RLY  |   |  0 -> HOLD_RLY  |   |  0 -> HOLD_RLY  |   |  1 -> HOLD_RLY  |   |  1 -> HOLD_RLY  |   |  0 -> HOLD_RLY  |     |-- HOLD_RLY
   |   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   __|
   |           |                     |                      |                     |                     |                     |
   |          \|/                   \|/                    \|/                   \|/                   \|/                   \|/           __
   |   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+     |
   |   | RCAD -> RING_RLY|   |  0 -> RING_RLY  |   |  0 -> RING_RLY  |   |  0 -> RING_RLY  |   |  0 -> RING_RLY  |   |  0 -> RING_RLY  |     |-- RING_RLY
   |   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   +-----------------+   __|
   |           |                     |                      |                     |                     |                     |
   |          \|/                   \|/                    \|/                   \|/                   \|/                   \|/           __
   |   +-------------------+ +-----------------+    +-----------------+  +-------------------+ +-------------------+  +-----------------+    |
   |   |ring_flash->LX_LAMP| |  0 -> LX_LAMP   |    |  1 -> LX_LAMP   |  |hold_flash->LX_LAMP| |hold_flash->LX_LAMP|  |  1 -> LX_LAMP   |    |-- L1/L2_LAMP
   |   +-------------------+ +-----------------+    +-----------------+  +-------------------+ +-------------------+  +-----------------+  __|
   |           |                     |                      |                     |                     |                     |
   |          \|/                   \|/                    \|/                   \|/                   \|/                   \|/
   |           |                     |                      |                     |                     |                     |
   +-----------o---------------------o----------------------o---------------------o---------------------o---------------------o


INTERLINK SYNC SIGNAL [CPU1]

    RB6:  _________________       ____________  12v
                           |     |
                           |_____|              GND
                            sync

    RB6 on both cpus are mostly in input mode with weak pullups enabled,
    looking for a ground-going sync signal from the "other" cpu. 
    
    If we haven't received a sync signal yet, when a full second (1000msec)
    has past, "we" switch RB5 to "output" mode and pull the output low, 
    and leave it low for a full iteration cycle. During this time we ignore
    signals from the other processor.
    
    During that time the "other" cpu should see the sync signal, and should
    sync to "us", as they are the slower processor.

    Either cpu can be the "slower" processor, and they may change places
    (depending on heat/environment), but the sync signal will always keep
    the slower processor in sync with the faster one.

    If both processors send sync signals at the same time it's OK;
    neither will see the other's signal because they're /already/ in sync.

    If one processor somehow "misses" the sync signal because the two
    processors are out of step just right, it's OK because they'll drift
    eventually, and sync as soon as they do.

    The sync of the two processors doesn't have to be perfect; it just has
    to be good enough to keep the flashing lights visibly in sync.
