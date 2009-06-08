#!/bin/sh
###############################################################################
#
# These commands completely changed ffmpeg from a useless tool to one that 
# seems to do anything it's asked.
#
# See http://fedoranews.org/cms/node/2853
#
###############################################################################

rpm -ivh http://rpm.livna.org/livna-release-7.rpm
yum -y install gstreamer-plugins-ugly ffmpeg
yum -y install xmms xmms-mp3 xmms-skins
yum -y install xine xine-lib libdvdcss xine-lib-extras-nonfree gxine
yum -y install vlc
yum -y remove totem totem-plparser
yum -y install totem-xine libdvdnav
yum -y install mplayer mplayer-gui mplayerplug-in
yum -y install amarok amarok-extras-nonfree

