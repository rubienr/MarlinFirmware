#!/bin/bash

echo "TODO rubienr: this board cannot be flashed via USB?"
exit 1

USB=`ls /dev/ttyACM* | tr ' ' '\n' | sort  | tail -1`

pio run --verbose --environment BIGTREE_SKR_PRO --target upload --jobs 9 --upload-port $USB
