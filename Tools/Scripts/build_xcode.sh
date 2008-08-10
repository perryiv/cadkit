#!/bin/sh

cd $HOME/src/cadkit/Tools/Scripts

./build_xcode_helios_release.sh
./build_xcode_minerva_release.sh
./build_xcode_nimue_release.sh
./build_xcode_helios_debug.sh
./build_xcode_minerva_debug.sh
./build_xcode_nimue_debug.sh
