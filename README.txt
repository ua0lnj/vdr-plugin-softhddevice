@file README.txt		@brief A software HD output device for VDR

Copyright (c) 2011 - 2013 by Johns.  All Rights Reserved.

Contributor(s):

License: AGPLv3

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

$Id$

A software and GPU emulated HD output device plugin for VDR.

    o Video decoder CPU / VA-API / VDPAU / CUVID
    o Video output VDPAU / GLX (VA-API CUVID)
    o Audio FFMpeg / Alsa / Analog
    o Audio FFMpeg / Alsa / Digital
    o Audio FFMpeg / OSS / Analog
    o HDMI/SPDIF pass-through
    o Software volume, compression, normalize and channel resample
    o VDR ScaleVideo API
    o Software deinterlacer Bob (VA-API only)
    o Autocrop
    o Grab image (VDPAU only)
    o Suspend / Dettach
    o Letterbox, Stretch and Center cut-out video display modes
    o atmo light support with plugin http://github.com/durchflieger/DFAtmo
    o PIP (Picture-in-Picture) (VDPAU only)

    o planned: Video output Opengl / Xv
    o planned: Improved software deinterlacer (yadif or/and ffmpeg filters)
    o XvBa support is no longer planned (use future Radeon UVD VDPAU)

To compile you must have the 'requires' installed.

Good luck

Quickstart:
-----------

Just type make and use.

Install:
--------
	1a) git
	original git
	git clone git://projects.vdr-developer.org/vdr-plugin-softhddevice.git
	latest git
	git clone https://github.com/ua0lnj/vdr-plugin-softhddevice.git
	cd vdr-plugin-softhddevice
	make
	make install

	2a) tarball

	Download original version from:
	    http://projects.vdr-developer.org/projects/plg-softhddevice/files

	tar vxf vdr-softhddevice-*.tar.bz2
	cd softhddevice-*
	make
	make install

	You can edit Makefile to enable/disable VDPAU / VA-API / CUVID / Alsa / OSS
	support.  The default is to autodetect as much as possible.

Setup:	environment
------
	Following is supported:

	DISPLAY=:0.0
		x11 display name
	NO_HW=1
		if set don't use the hardware decoders
	NO_MPEG_HW=1
		if set don't use the hardware decoder for mpeg1/2

    only if alsa is configured
	ALSA_DEVICE=default
		alsa PCM device name
	ALSA_PASSTHROUGH_DEVICE=
		alsa pass-though (AC-3,E-AC-3,DTS,...) device name
	ALSA_MIXER=default
		alsa control device name
	ALSA_MIXER_CHANNEL=PCM
		alsa control channel name

    only if oss is configured
	OSS_AUDIODEV=/dev/dsp
		oss dsp device name
	OSS_PASSTHROUGHDEV=
		oss pass-though (AC-3,E-AC-3,DTS,...) device name
	OSS_MIXERDEV=/dev/mixer
		oss mixer device name
	OSS_MIXER_CHANNEL=pcm
		oss mixer channel name

Setup: /etc/vdr/setup.conf
------
	Following is supported:

	softhddevice.MakePrimary = 0
	0 = no change, 1 make softhddevice primary at start

	softhddevice.HideMainMenuEntry = 0
	0 = show softhddevice main menu entry, 1 = hide entry

	softhddevice.Osd.Width = 0
	0 = auto (=display, unscaled) n = fixed osd size scaled for display
	softhddevice.Osd.Height = 0
	0 = auto (=display, unscaled) n = fixed osd size scaled for display

	<res> of the next parameters is 576i, 720p, 1080i_fake or 1080i.
	1080i_fake is 1280x1080 or 1440x1080
	1080i is "real" 1920x1080

	softhddevice.<res>.Scaling = 0
	0 = normal, 1 = fast, 2 = HQ, 3 = anamorphic

	softhddevice.<res>.Deinterlace = 0
	0 = bob, 1 = weave, 2 = temporal, 3 = temporal_spatial, 4 = software
	(only 0, 1, 4 supported with VA-API)

	softhddevice.<res>.SkipChromaDeinterlace = 0
	0 = disabled, 1 = enabled (for slower cards, poor qualität)

	softhddevice.<res>.InverseTelecine = 0
	0 = disabled, 1 = enabled

	softhddevice.<res>.Denoise = 0
	0 .. 1000 noise reduction level (0 off, 1000 max)

	softhddevice.<res>.Sharpness = 0
	-1000 .. 1000 noise reduction level (0 off, -1000 max blur,
	    1000 max sharp)

	softhddevice.<res>.CutTopBottom = 0
	Cut 'n' pixels at at top and bottom of the video picture.

	softhddevice.<res>.CutLeftRight = 0
	Cut 'n' pixels at at left and right of the video picture.

	softhddevice.AudioDelay = 0
	+n or -n ms
	delay audio or delay video

	softhddevice.AudioPassthrough = 0
	0 = none, 1 = PCM, 2 = MPA, 4 = AC-3, 8 = EAC-3, -X disable

	for PCM/AC-3/EAC-3 the pass-through device is used and the audio
	stream is passed undecoded to the output device.
	z.b. 12 = AC-3+EAC-3, 13 = PCM+AC-3+EAC-3
	note: MPA/DTS/TrueHD/... aren't supported yet
	negative values disable passthrough

	softhddevice.AudioDownmix = 0
	0 = none, 1 = downmix
	Use ffmpeg/libav downmix of AC-3/EAC-3 audio to stereo.

	softhddevice.AudioSoftvol = 0
	0 = off, use hardware volume control
	1 = on, use software volume control

	softhddevice.AudioNormalize = 0
	0 = off, 1 = enable audio normalize

	softhddevice.AudioMaxNormalize = 0
	maximal volume factor/1000 of the normalize filter

	softhddevice.AudioCompression = 0
	0 = off, 1 = enable audio compression

	softhddevice.AudioMaxCompression = 0
	maximal volume factor/1000 of the compression filter

	softhddevice.AudioStereoDescent = 0
	reduce volume level (/1000) for stereo sources

	softhddevice.AudioBufferTime = 0
	0 = default (336 ms)
	1 - 1000 = size of the buffer in ms

	softhddevice.AutoCrop.Interval = 0
	0 disables auto-crop
	n each 'n' frames auto-crop is checked.

	softhddevice.AutoCrop.Delay = 0
	if auto-crop is over 'n' intervals the same, the cropping is
	used.

	softhddevice.AutoCrop.Tolerance = 0
	if detected crop area is too small, cut max 'n' pixels at top and
	bottom.

	softhddevice.Background = 0
	32bit RGBA background color
	(Red * 16777216 +  Green * 65536 + Blue * 256 + Alpha)
	or hex RRGGBBAA
	grey 127 * 16777216 + 127 * 65536 + 127 * 256 => 2139062016
	in the setup menu this is entered as (24bit RGB and 8bit Alpha)
	(Red * 65536 +  Green * 256 + Blue)

	softhddevice.StudioLevels = 0
		0 use PC levels (0-255) with vdpau.
		1 use studio levels (16-235) with vdpau.

	softhddevice.Suspend.Close = 0
	1 suspend closes x11 window, connection and audio device.
	(use svdrpsend plug softhddevice RESU to resume, if you have no lirc)

	softhddevice.Suspend.X11 = 0
	1 suspend stops X11 server (not working yet)

	softhddevice.60HzMode = 0
	0 disable 60Hz display mode
	1 enable 60Hz display mode

	softhddevice.SoftStartSync = 0
	0 disable soft start of audio/video sync
	1 enable soft start of audio/video sync

	softhddevice.BlackPicture = 0
	0 disable black picture during channel switch
	1 enable black picture during channel switch

	softhddevice.ClearOnSwitch = 0
	0 keep video und audio buffers during channel switch
	1 clear video and audio buffers on channel switch

	softhddevice.Video4to3DisplayFormat = 1
	0 pan and scan
	1 letter box
	2 center cut-out

	softhddevice.VideoOtherDisplayFormat = 1
	0 pan and scan
	1 pillar box
	2 center cut-out

	softhddevice.pip.X = 79
	softhddevice.pip.Y = 78
	softhddevice.pip.Width = 18
	softhddevice.pip.Height = 18
	PIP pip window position and size in percent.

	softhddevice.pip.VideoX = 0
	softhddevice.pip.VideoY = 0
	softhddevice.pip.VideoWidth = 0
	softhddevice.pip.VideoHeight = 0
	PIP video window position and size in percent.

	softhddevice.pip.Alt.X = 0
	softhddevice.pip.Alt.Y = 50
	softhddevice.pip.Alt.Width = 0
	softhddevice.pip.Alt.Height = 50
	PIP alternative pip window position and size in percent.

	softhddevice.pip.Alt.VideoX = 0
	softhddevice.pip.Alt.VideoY = 0
	softhddevice.pip.Alt.VideoWidth = 0
	softhddevice.pip.Alt.VideoHeight = 50
	PIP alternative video window position and size in percent.


Setup: /etc/vdr/remote.conf
------

	Add "XKeySym." definitions to /etc/vdr/remote.conf to control
	the vdr and plugin with the connected input device.

	fe.
	XKeySym.Up	Up
	XKeySym.Down	Down
	...

	Additional to the x11 input sends the window close button "Close".

	fe.
	XKeySym.Power	Close

Commandline:
------------

	Use vdr -h to see the command line arguments supported by the plugin.

    -a audio_device	Selects audio output module and device.
	""		to disable audio output
	/...		to use oss audio module (if compiled with oss
			support)
	other		to use alsa audio module (if compiled with alsa
			support)

    -p device		audio device for pass-through (hw:0,1 or /dev/dsp1)
    -c channel		audio mixer channel name (fe. PCM)
    -d display		display of x11 server (fe. :0.0)
    -f 			start with fullscreen window (only with window manager)
    -g geometry		x11 window geometry wxh+x+y
    -l loglevel		set the log level (0=none, 1=errors, 2=info, 3=debug)
    -v device		video driver device (va-api, va-api-glx, vdpau, cuvid, noop)
    -s 			start in suspended mode
    -x 			start x11 server, with -xx try to connect, if this fails
    -X args		X11 server arguments (f.e. -nocursor)

    -w workaround 	enable/disable workarounds:
	no-hw-decoder			disable hw decoder, use software decoder only
	no-mpeg-hw-decoder		disable hw decoder for mpeg only
	still-hw-decoder		enable hardware decoder for still-pictures
	still-h264-hw-decoder		enable h264 hw decoder for still-pictures
	alsa-driver-broken		disable broken alsa driver message
	alsa-no-close-open		disable close open to fix alsa no sound bug
	alsa-close-open-delay		enable close open delay to fix no sound bug
	ignore-repeat-pict		disable repeat pict message
	use-possible-defect-frames	prefer faster channel switch
	disable-ogl-osd			disable openGL osd

    -D 			start in detached mode


SVDRP:
------

	Use 'svdrpsend.pl plug softhddevice HELP'
	or 'svdrpsend plug softhddevice HELP' to see the SVDRP commands help
	and which are supported by the plugin.

Keymacros:
----------

	See keymacros.conf how to setup the macros.

	This are the supported key sequences:

	@softhddevice Blue 1 0		disable pass-through
	@softhddevice Blue 1 1		enable pass-through
	@softhddevice Blue 1 2		toggle pass-through
	@softhddevice Blue 1 3		decrease audio delay by 10ms
	@softhddevice Blue 1 4		increase audio delay by 10ms
	@softhddevice Blue 1 5		toggle ac3 mixdown
	@softhddevice Blue 2 0		disable fullscreen
	@softhddevice Blue 2 1		enable fullscreen
	@softhddevice Blue 2 2		toggle fullscreen
	@softhddevice Blue 2 3		disable auto-crop
	@softhddevice Blue 2 4		enable auto-crop
	@softhddevice Blue 2 5		toggle auto-crop
	@softhddevice Blue 3 0		stretch 4:3 to 16:9
	@softhddevice Blue 3 1		letter box 4:3 in 16:9
	@softhddevice Blue 3 2		center cut-out 4:3 to 16:9
	@softhddevice Blue 3 9		rotate 4:3 to 16:9 zoom mode

Running:
--------

	Click into video window to toggle fullscreen/window mode, only if you
	have a window manager running.

Warning:
--------
	libav is not supported, expect many bugs with it.

Known Bugs:
-----------
	VA-API doesn't v-sync h264 interlaced streams

Requires:
---------
	media-video/vdr (version >=1.7.xx)
		Video Disk Recorder - turns a pc into a powerful set top box
		for DVB.
		http://www.tvdr.de/

	media-video/ffmpeg (version >=0.7)
		Complete solution to record, convert and stream audio and
		video. Includes libavcodec and libswresample.
		http://ffmpeg.org
	media-libs/alsa-lib
		Advanced Linux Sound Architecture Library
		http://www.alsa-project.org
    or
	kernel support for oss/oss4 or alsa oss emulation

	x11-libs/libva (deprecated)
		Video Acceleration (VA) API for Linux
		http://www.freedesktop.org/wiki/Software/vaapi
	x11-libs/libva-intel-driver
		HW video decode support for Intel integrated graphics
		http://www.freedesktop.org/wiki/Software/vaapi
    or
	x11-libs/vdpau-video
		VDPAU Backend for Video Acceleration (VA) API
		http://www.freedesktop.org/wiki/Software/vaapi
    or
	x11-libs/xvba-video
		XVBA Backend for Video Acceleration (VA) API
		http://www.freedesktop.org/wiki/Software/vaapi

	x11-libs/libvdpau
		VDPAU wrapper and trace libraries
		http://www.freedesktop.org/wiki/Software/VDPAU
    or
	cuvid libs
	    ffnvcodec and ffmpeg with cuvid support
	    libs gl glu

	x11-libs/libxcb,
		X C-language Bindings library
		http://xcb.freedesktop.org
	x11-libs/xcb-util,
	x11-libs/xcb-util-wm,
	x11-libs/xcb-util-keysyms
		X C-language Bindings library
		http://xcb.freedesktop.org
		Only versions >= 0.3.8 are good supported

	x11-libs/libX11
		X.Org X11 library
		http://xorg.freedesktop.org

	GNU Make 3.xx
		http://www.gnu.org/software/make/make.html

Optional:
	for openGL osd need
	    libs gl glu glew freetype2

