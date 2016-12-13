#!/bin/bash
# Adds the required files to the provided disk.dsk
# usage: add_to_disk PATH_TO_APPLECOMMANDER.jar

set -e

if (( $# != 3 )); then
    echo "Bad number of arguments"
    echo "usage: add_to_disk.sh PATH_TO_APPLECOMMANDER.jar PATH_TO_BINARY.a2 PATH_TO_DISK"
    exit
fi

echo " . revoving previous instance of GOL form the disk"
java -jar ${1} -d ${3} GOL

echo " .. adding GOL to the disk"
java -jar ${1} -cc65 ${3} GOL BIN < ${2}

echo "DONE."
