#!/bin/bash
# Adds the gol.a2 Apple2 binary exceutable to the provided disk.dsk
# usage: add_to_disk PATH_TO_APPLECOMMANDER.jar

set -e
 
if (( $# != 1 )); then
    echo "Bad number of arguments"
    echo "usage: add_to_disk.sh PATH_TO_APPLECOMMANDER.jar"
    exit
fi

echo " . revoving previous instance of GOL form the disk"
java -jar ${1} -d disk.dsk GOL

echo " .. adding GOL to the disk"
java -jar ${1} -cc65 disk.dsk GOL BIN < gol.a2

echo "DONE."
