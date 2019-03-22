#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = softhddevice

### Configuration (edit this for your needs)

    # support alsa audio output module
ALSA ?= $(shell pkg-config --exists alsa && echo 1)
    # support OSS audio output module
OSS ?= 1
    # support VDPAU video output module
VDPAU ?= $(shell pkg-config --exists vdpau && echo 1)
    # support VA-API video output module (deprecated)
VAAPI ?= $(shell pkg-config --exists libva && echo 1)
    # support glx output
OPENGL ?= $(shell pkg-config --exists gl glu && echo 1)
    # screensaver disable/enable
SCREENSAVER ?= 1
    # use ffmpeg libswscale
SWSCALE ?= $(shell pkg-config --exists libswscale && echo 1)
    # use ffmpeg libswresample
SWRESAMPLE ?= $(shell pkg-config --exists libswresample && echo 1)
    # use libav libavresample
AVRESAMPLE ?= $(shell pkg-config --exists libavresample && echo 1)
ifeq ($(SWRESAMPLE),1)
ifeq ($(AVRESAMPLE),1)
$(info WARNING, you have libavresample and libswresample together!!!)
    # if you got a segfault, try changing the comment in following lines
SWRESAMPLE = 0
#AVRESAMPLE = 0
endif
endif
    # support CUVID (NVdec) video decoder with vdpau render
ifeq ($(shell lspci|grep VGA|grep -c NVIDIA),1)
ifeq ($(VDPAU),1)
CUVID ?= $(shell ffmpeg -loglevel quiet -decoders | grep -c hevc_cuvid)
endif
endif

#CONFIG := -DDEBUG 
#-DOSD_DEBUG	# enable debug output+functions
#CONFIG += -DSTILL_DEBUG=2		# still picture debug verbose level

CONFIG += -DAV_INFO -DAV_INFO_TIME=3000	# info/debug a/v sync
CONFIG += -DUSE_PIP			# PIP support
CONFIG += -DHAVE_PTHREAD_NAME		# supports new pthread_setname_np
CONFIG += -DUSE_TS			# build ts parser
CONFIG += -DUSE_MPEG_COMPLETE		# support only complete mpeg packets
CONFIG += -DH264_EOS_TRICKSPEED		# insert seq end packets for trickspeed
#CONDIF += -DDUMP_TRICKSPEED		# dump trickspeed packets
#CONFIG += -DUSE_BITMAP			# VDPAU, use bitmap surface for OSD
CONFIG += -DUSE_VDR_SPU			# use VDR SPU decoder.
#CONFIG += -DUSE_SOFTLIMIT		# (tobe removed) limit the buffer fill

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*const VERSION *=' $(PLUGIN).cpp | awk '{ print $$7 }' | sed -e 's/[";]//g')
GIT_REV = $(shell git describe --always 2>/dev/null)

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG = $(call PKGCFG,plgcfg)
#
TMPDIR ?= /tmp

### The compiler options:

export CFLAGS	= $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

ifeq ($(CFLAGS),)
$(warning CFLAGS not set)
endif
ifeq ($(CXXFLAGS),)
$(warning CXXFLAGS not set)
endif

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)

### Allow user defined options to overwrite defaults:

-include $(PLGCFG)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:

SOFILE = libvdr-$(PLUGIN).so

### Parse softhddevice config

ifeq ($(ALSA),1)
CONFIG += -DUSE_ALSA
_CFLAGS += $(shell pkg-config --cflags alsa)
LIBS += $(shell pkg-config --libs alsa)
endif
ifeq ($(OSS),1)
CONFIG += -DUSE_OSS
endif
ifeq ($(VDPAU),1)
CONFIG += -DUSE_VDPAU
_CFLAGS += $(shell pkg-config --cflags vdpau)
LIBS += $(shell pkg-config --libs vdpau)
endif
ifeq ($(VAAPI),1)
CONFIG += -DUSE_VAAPI
_CFLAGS += $(shell pkg-config --cflags libva-x11 libva)
LIBS += $(shell pkg-config --libs libva-x11 libva)
ifeq ($(OPENGL),1)
_CFLAGS += $(shell pkg-config --cflags libva-glx)
LIBS += $(shell pkg-config --libs libva-glx)
endif
endif
ifeq ($(OPENGL),1)
CONFIG += -DUSE_GLX
_CFLAGS += $(shell pkg-config --cflags gl glu)
LIBS += $(shell pkg-config --libs gl glu)
endif
ifeq ($(SCREENSAVER),1)
CONFIG += -DUSE_SCREENSAVER
_CFLAGS += $(shell pkg-config --cflags xcb-screensaver xcb-dpms)
LIBS += $(shell pkg-config --libs xcb-screensaver xcb-dpms)
endif
ifeq ($(SWSCALE),1)
CONFIG += -DUSE_SWSCALE
_CFLAGS += $(shell pkg-config --cflags libswscale)
LIBS += $(shell pkg-config --libs libswscale)
endif
ifeq ($(SWRESAMPLE),1)
CONFIG += -DUSE_SWRESAMPLE
_CFLAGS += $(shell pkg-config --cflags libswresample)
LIBS += $(shell pkg-config --libs libswresample)
endif
ifeq ($(AVRESAMPLE),1)
CONFIG += -DUSE_AVRESAMPLE
_CFLAGS += $(shell pkg-config --cflags libavresample)
LIBS += $(shell pkg-config --libs libavresample)
endif
ifeq ($(CUVID),1)
CONFIG += -DCUVID
endif

_CFLAGS += $(shell pkg-config --cflags libavcodec x11 x11-xcb xcb xcb-icccm)
LIBS += -lrt $(shell pkg-config --libs libavcodec x11 x11-xcb xcb xcb-icccm)

### Includes and Defines (add further entries here):

INCLUDES +=

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"' -D_GNU_SOURCE $(CONFIG) \
	$(if $(GIT_REV), -DGIT_REV='"$(GIT_REV)"')

### Make it standard

override CXXFLAGS += $(_CFLAGS) $(DEFINES) $(INCLUDES) \
    -g -W -Wall -Wextra -Winit-self -Werror=overloaded-virtual
override CFLAGS	  += $(_CFLAGS) $(DEFINES) $(INCLUDES) \
    -g -W -Wall -Wextra -Winit-self -Wdeclaration-after-statement

### The object files (add further files here):

OBJS = $(PLUGIN).o softhddev.o video.o audio.o codec.o ringbuffer.o

SRCS = $(wildcard $(OBJS:.o=.c)) $(PLUGIN).cpp

### The main target:

all: $(SOFILE) i18n

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(SRCS) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR	  = po
I18Npo	  = $(wildcard $(PODIR)/*.po)
I18Nmo	  = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot	  = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(SRCS)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP \
	-k_ -k_N --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) \
	--msgid-bugs-address='<see README>' -o $@ `ls $^`

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	install -D -m644 $< $@

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)

### Targets:

$(OBJS): Makefile

$(SOFILE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) $(LIBS) -o $@

install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install: install-lib install-i18n

dist: $(I18Npo) clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~

## Private Targets:

HDRS=	$(wildcard *.h)

indent:
	for i in $(SRCS) $(HDRS); do \
		indent $$i; \
		unexpand -a $$i | sed -e s/constconst/const/ > $$i.up; \
		mv $$i.up $$i; \
	done

video_test: video.c Makefile
	$(CC) -DVIDEO_TEST -DVERSION='"$(VERSION)"' $(CFLAGS) $(LDFLAGS) $< \
	$(LIBS) -o $@
