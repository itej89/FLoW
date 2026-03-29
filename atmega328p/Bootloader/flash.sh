#! /bin/bash

FULL_PATH=$(realpath "./build/blAT328pTWI.hex")
avrdude -c usbasp -p m328p -U flash:w:"$FULL_PATH":i