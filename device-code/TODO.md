## Features

## Architecture

### client-server protocol


states

reset -> go to stopped

stopped (waiting for start command)

running (emit data) (stop -> reset)

watchdog -> reset


commands 

- start
- stop
- restart  (used if camera data not available)
- info (generate report on available devices, can be used when stopped)


heart-beat

Both server and client are required to generate regular messages unless the device is in the **stopped** state.


### Rationale

It can be frustrating if a device freezes or keeps restarting, as it can be hard to establish why.
The client is responsible for re-starting if the device isn't behaving as expected.


## Refactor


## Libraries

https://developer.sony.com/develop/spresense/developer-tools/api-reference/api-references-arduino/

LowPower  - can also measure current

