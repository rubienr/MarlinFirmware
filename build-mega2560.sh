#!/bin/bash

USB=`ls /dev/ttyUSB* | tr ' ' '\n' | sort  | tail -1`

pio run --environment mega2560 --target upload --jobs 3 --upload-port $USB
