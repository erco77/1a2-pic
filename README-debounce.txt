 The debounce structure has variables to implement digital signal noise rejection
 and "snap-action" hysteresis to avoid oscillations during transitions.

 The structure is implemented as 6 integer values; 2 dynamic that change with live data,
 and 4 constants that are set on initialization, and don't change thereafter:

        typedef struct {
            int value;          // current running value counter (in msecs)
            int max_value;      // (const) 'value' will be clamped to this maximum
            int on_thresh;      // (const) 'value' must count above this to "snap on"
            int off_thresh;     // (const) 'value' must count below this to "snap off"
            int thresh;         // 'current' threshold value implementing 'snap-action' hysteresis
        } Debounce;

 Consider the following noisy digital input signal:

  DIGTITAL INPUT SIGNAL:
                         _      ___   _______________       __          _
                        | | ||||   | |               | ||| |  || |     | |
             ___________| |_||||   |_|               |_|||||  ||_|_____| |______


 We want to clean that up, rejecting the noise with an average weighting,
 and hysteresis to snap on or off to prevent quick oscillations, so that we get:

  DESIRED OUTPUT DIGITAL SIGNAL:
                                    _______________________________
                                   |                               |
             ______________________|                               |____________


 To do this, we create a signal chaser for the digital input signal; a value that
 might represent either voltage over time, or perhaps better, represents a time tally
 of how long the input digital signal is hi or low.

 We use time, so that the running 'value' represents milliseconds the signal was either
 on or off.

 So in the sample loop for the digital input signal, we count the value up or down by
 the number of millisecs between samples, depending on if the input signal is high or low.
 The result is an integrator of sorts:

      DIGTITAL INPUT SIGNAL:
                         _      ___   _______________       __          _
                        | | ||||   | |               | ||| |  || |     | |
             ___________| |_||||   |_|               |_|||||  ||_|_____| |______

                       .<-- Noise -->.               .<----- Noise ----->.

      INTEGRATOR SIGNAL:

                                        _____________        _                      -- max value
                                   ^   /             \      / \/\_
                                  / \ /               \/\/\/      \
                                 /   v                             \     _
                         -     _/                                   \   / \
             ___________/ \_/\/                                      \_/   \____    __ 0


 In our 'Debounce' structure, this integrator is the integer 'value', which is clamped
 to the range 0 to 'max_value'. We then pre-determine, typically empirioally, the optimal
 on/off threshold values for snapping our output digital signal on and off:

      INPUT SIGNAL:      _      ___   _______________       __          _
                        | | ||||   | |               | ||| |  || |     | |
             ___________| |_||||   |_|               |_|||||  ||_|_____| |______
                       .           . .               .                   .
                       .<-- Noise -->.               .<----- Noise ----->.
                       .           . .               .                   .
                       .           . .
                                   .
                                   .
      INTEGRATOR:                  .    _____________        _
                                   ^   /             \      / \/\_
  on thresh  - - - - - - - - - - -/ \ /- - - - - - - -\/\/\/ - - -\ - - - - - - - on thresh
                                 / . v                             \     _
  off thresh - - - - - - - - - _/- . - - - - - - - - - - - - - - - -\ - / \ - - - off thresh
             ___________/ \_/\/    .                               . \_/   \____
                                   .<-- hits "on" threshold        .
                                   .                               .<-- hits "off" threshold
                                   .                               .
                                   .                               .
      OUTPUT SIGNAL:               ._______________________________.
                                   |                               |
             ______________________|                               |____________


  To implement hysteresis, the Debounce structure uses the other dynamic variable 'thresh'
  to use as the current 'crossing value'. When the signal is low, thresh is set to the 'on threshold',
  which is what the integrator must reach to transition the output to high. Once reached, immediately
  the 'thresh' value is set to the 'off threshold' value, so that the intgegrator must now go BELOW
  that value to transition the output signal back to low.

