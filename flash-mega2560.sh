#!/bin/bash

USB=`ls /dev/ttyUSB* | tr ' ' '\n' | sort  | tail -1`

pio run --verbose --environment mega2560 --target upload --jobs 9 --upload-port $USB
