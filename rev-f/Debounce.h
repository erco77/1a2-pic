//
// Debounce Module -- Digital signal processing: removes digital noise and adds hysteresis
//                    See README-debounce.txt for more info on how this works.

// Generic signal debouncing struct
//     Modularizes timing data for noise reduction of individual inputs.
//     Create one instance of this struct for each digital input that needs
//     noise reduction. Recommended initial values shown in (parens), but
//     should be customized for the needs of each input's noise profile.
//
typedef struct {
    int value;          // (0)  current running value counter (in msecs)
    int max_value;      // (20) 'value' will be clamped to this maximum
    int on_thresh;      // (15) 'value' must count above this to "snap on"
    int off_thresh;     // (8)  'value' must count below this to "snap off"
    int thresh;         // (15) 'current' threshold value implementing 'snap-action' hysteresis
} Debounce;

// Debounce the raw, noisy, digital hardware 'input'
//     Returns the clean state of the input.
//     Call this only once per main-loop iteration to ensure accurate timing.
//
int DebounceNoisyInput(Debounce *d, int input) {
    extern const long G_msecs_per_iter;		// extern global: millisecs per main() loop iterations

    // Debounce the digital hardware input
    if ( input ) {
        // Hardware bit ON? Might be noise, debounce..
        if ( d->value < d->max_value ) {
            d->value += G_msecs_per_iter;
            if ( d->value > d->on_thresh ) {
                // SNAP ON. No longer "off" until above off_thresh
                d->thresh = d->off_thresh;
            }
        }
    } else {
        // Hardware bit OFF? Might be noise, debounce..
        if ( d->value > 0 ) {
            d->value -= G_msecs_per_iter;
            if ( d->value < d->off_thresh ) {
                // SNAP OFF. No longer "on" until above on_thresh
                d->thresh = d->on_thresh;
            }
        }
    }
    return( (d->value > d->thresh) ? 1 : 0);
}
