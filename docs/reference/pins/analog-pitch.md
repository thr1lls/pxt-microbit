# Analog Pitch

Sends a pulse-width modulation (PWM) signal to the pin ``P0``.
Use [analog set pitch pin](/reference/pins/analog-set-pitch-pin) to set the current pitch pin.

```sig
pins.analogPitch(440, 300)
```

## Parameters

* **frequency**: a [number](/types/number) which is the frequency of the PWM signal at the pitch pin.
* **ms**: a [number](/types/number) in milliseconds that is the duration of the signal at the pitch pin.

## Example

Set the pitch pin to `P1` and send a 440 Hz tone for 1 second.

```blocks
pins.analogSetPitchPin(AnalogPin.P0);
let frequency1 = 440
let duration = 1000
pins.analogSetPitchPin(AnalogPin.P1);
pins.analogPitch(frequency1, duration)
```

## Some common notes

* 440 = A4 on piano
* see [piano key frequencies ](https://en.wikipedia.org/wiki/Piano_key_frequencies) for more information

## See also

[@boardname@ pins](/device/pins), [analog set period](/reference/pins/analog-set-period), [analog set pitch pin](/reference/pins/analog-set-pitch-pin)

