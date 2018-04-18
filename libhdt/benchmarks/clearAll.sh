#!/usr/bin/env bash

data=data/

currentdir=`pwd`
pushd "$(dirname "$0")" >/dev/null

# Get all files NOT ending in nt
echo
echo "Removing output files in $data directory":
for filename in `find data/* -not -name "*.nt"`; do
    echo "Deleting: "$filename
    rm $filename
done
echo "Done"

popd >/dev/null