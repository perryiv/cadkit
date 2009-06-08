#!/bin/sh

cd $HOME/src/cadkit
svn update
xcodebuild -project ./Xcode/Helios.xcodeproj/ -target HeliosQtApp -configuration Release OBJROOT=$HOME/src/bin SYMROOT=$HOME/src/bin

