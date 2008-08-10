#!/bin/sh
cd $HOME/src/cadkit/Xcode
xcodebuild -project Helios_xcode3.xcodeproj -configuration Release -target HeliosQt OBJROOT=$HOME/src/bin SYMROOT=$HOME/src/bin
