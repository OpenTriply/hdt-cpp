#!/usr/bin/env bash

data=data/

currentdir=`pwd`
pushd "$(dirname "$0")" >/dev/null

# Get all files NOT ending in nt
echo
`find ./ -name '*index*' | xargs rm`
`find ./ -name 'cat*.hdt' | xargs rm`


echo "Done"

popd >/dev/null
