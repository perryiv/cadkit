#!/bin/sh
cd $HOME/src/cadkit/Xcode
xcodebuild -project Helios_xcode3.xcodeproj -configuration Debug -target Nimue OBJROOT=$HOME/src/bin SYMROOT=$HOME/src/bin
