///
///	@file video.h	@brief Video module header file
///
///	Copyright (c) 2009 - 2015 by Johns.  All Rights Reserved.
///
///	Contributor(s):
///
///	License: AGPLv3
///
///	This program is free software: you can redistribute it and/or modify
///	it under the terms of the GNU Affero General Public License as
///	published by the Free Software Foundation, either version 3 of the
///	License.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU Affero General Public License for more details.
///
///	$Id$
//////////////////////////////////////////////////////////////////////////////

/// @addtogroup Video
/// @{

//----------------------------------------------------------------------------
//	Typedefs
//----------------------------------------------------------------------------

    /// Video hardware decoder typedef
typedef struct _video_hw_decoder_ VideoHwDecoder;

    /// Video output stream typedef
typedef struct __video_stream__ VideoStream;

    /// Video resolutions selector
typedef enum _video_resolutions_
{
    VideoResolution576i,		///< ...x576 interlaced
    VideoResolution720p,		///< ...x720 progressive
    VideoResolutionFake1080i,		///< 1280x1080 1440x1080 interlaced
    VideoResolution1080i,		///< 1920x1080 interlaced
    VideoResolutionUHD,			///< UHD progressive
    VideoResolutionMax			///< number of resolution indexs
} VideoResolutions;

//----------------------------------------------------------------------------
//	Variables
//----------------------------------------------------------------------------
enum  VideoHardwareDecoderMode {
    HWOff = 0,
    HWmpeg2Off,
    HWhevcOff,
    HWOn,
};

enum VideoOutParameters {
    brightness,
    contrast,
    saturation,
    hue,
    stde,
};

extern enum VideoHardwareDecoderMode VideoHardwareDecoder;	///< flag use hardware decoder
extern char VideoIgnoreRepeatPict;	///< disable repeat pict warning
extern int VideoAudioDelay;		///< audio/video delay
extern char ConfigStartX11Server;	///< flag start the x11 server

//----------------------------------------------------------------------------
//	Prototypes
//----------------------------------------------------------------------------

    /// Allocate new video hardware decoder.
extern VideoHwDecoder *VideoNewHwDecoder(VideoStream *);

    /// Deallocate video hardware decoder.
extern void VideoDelHwDecoder(VideoHwDecoder *);

#ifdef LIBAVCODEC_VERSION
    /// Get and allocate a video hardware surface.
extern unsigned VideoGetSurface(VideoHwDecoder *, const AVCodecContext *);

    /// Release a video hardware surface
extern void VideoReleaseSurface(VideoHwDecoder *, unsigned);

extern void VideoUnregisterSurface(VideoHwDecoder *);
    /// Callback to negotiate the PixelFormat.
extern enum AVPixelFormat Video_get_format(VideoHwDecoder *, AVCodecContext *,
    const enum AVPixelFormat *);

    /// Render a ffmpeg frame.
extern void VideoRenderFrame(VideoHwDecoder *, const AVCodecContext *,
    const AVFrame *);

    /// Get hwaccel context for ffmpeg.
extern void *VideoGetHwAccelContext(VideoHwDecoder *);

#if LIBAVUTIL_VERSION_MAJOR < 56
#ifdef AVCODEC_VDPAU_H
    /// Draw vdpau render state.
extern void VideoDrawRenderState(VideoHwDecoder *,
    struct vdpau_render_state *);
#endif
#endif
#endif

    /// Poll video events.
extern void VideoPollEvent(void);

#ifdef USE_OPENGLOSD
    /// Set callback funktion to notify VDR about VideoEvents
extern void VideoSetVideoEventCallback(void (*)(void));
#endif

    /// Wakeup display handler.
extern void VideoDisplayWakeup(void);

    /// Set video device.
extern void VideoSetDevice(const char *);

    /// Get video driver name.
extern const char *VideoGetDriverName(void);

    /// Get used video driver.
extern int VideoIsDriverVdpau(void);
extern int VideoIsDriverVaapi(void);
extern int VideoIsDriverCuvid(void);

    /// Set video geometry.
extern int VideoSetGeometry(const char *);

    /// Set 60Hz display mode.
extern void VideoSet60HzMode(int);

    /// Set soft start audio/video sync.
extern void VideoSetSoftStartSync(int);

    /// Set show black picture during channel switch.
extern void VideoSetBlackPicture(int);

    /// Set brightness adjustment.
extern void VideoSetBrightness(int);

    /// Get brightness configurations.
extern int VideoGetBrightnessConfig(int *minvalue, int *defvalue, int *maxvalue);

    /// Set contrast adjustment.
extern void VideoSetContrast(int);

    /// Get contrast configurations.
extern int VideoGetContrastConfig(int *minvalue, int *defvalue, int *maxvalue);

    /// Set saturation adjustment.
extern void VideoSetSaturation(int);

    /// Get saturation configurations.
extern int VideoGetSaturationConfig(int *minvalue, int *defvalue, int *maxvalue);

    /// Set hue adjustment.
extern void VideoSetHue(int);

    /// Get hue configurations.
extern int VideoGetHueConfig(int *minvalue, int *defvalue, int *maxvalue);

    /// Set skin tone enhancement.
extern void VideoSetSkinToneEnhancement(int);

    /// Get skin tone enhancement configurations.
extern int VideoGetSkinToneEnhancementConfig(int *minvalue, int *defvalue, int *maxvalue);

    /// Set video output position.
extern void VideoSetOutputPosition(VideoHwDecoder *, int, int, int, int);

    /// Set video mode.
extern void VideoSetVideoMode(int, int, int, int);

    /// Set 4:3 display format.
extern void VideoSet4to3DisplayFormat(int);

    /// Set other display format.
extern void VideoSetOtherDisplayFormat(int);

    /// Set video fullscreen mode.
extern void VideoSetFullscreen(int);

    /// Get scaling modes.
extern int VideoGetScalingModes(const char* **long_table, const char* **short_table);

    /// Get deinterlace modes.
extern int VideoGetDeinterlaceModes(const char* **long_table, const char* **short_table);

    /// Set deinterlace.
extern void VideoSetDeinterlace(int[VideoResolutionMax]);

    /// Set skip chroma deinterlace.
extern void VideoSetSkipChromaDeinterlace(int[VideoResolutionMax]);

    /// Set inverse telecine.
extern void VideoSetInverseTelecine(int[VideoResolutionMax]);

    /// Set scaling.
extern void VideoSetScaling(int[VideoResolutionMax]);

    /// Set denoise.
extern void VideoSetDenoise(int[VideoResolutionMax]);

    /// Get denoise configurations.
extern int VideoGetDenoiseConfig(int *minvalue, int *defvalue, int *maxvalue);

    /// Set sharpen.
extern void VideoSetSharpen(int[VideoResolutionMax]);

    /// Get sharpen configurations.
extern int VideoGetSharpenConfig(int *minvalue, int *defvalue, int *maxvalue);

    /// Set cut top and bottom.
extern void VideoSetCutTopBottom(int[VideoResolutionMax]);

    /// Set cut left and right.
extern void VideoSetCutLeftRight(int[VideoResolutionMax]);

    /// Set first & second field ordering.
extern void VideoSetFirstField(int[VideoResolutionMax]);
extern void VideoSetSecondField(int[VideoResolutionMax]);

    /// Set studio levels.
extern void VideoSetStudioLevels(int);

    /// Set background.
extern void VideoSetBackground(uint32_t);

    /// Set audio delay.
extern void VideoSetAudioDelay(int);

    /// Set auto-crop parameters.
extern void VideoSetAutoCrop(int, int, int);

    /// Clear OSD.
extern void VideoOsdClear(void);

    /// Draw an OSD ARGB image.
extern void VideoOsdDrawARGB(int, int, int, int, int, const uint8_t *, int,
    int);

    /// Activate displaying OSD
void ActivateOsd(void);
#ifdef USE_VDPAU
    /// Get VDPAU DEVICE
extern void *GetVDPAUDevice(void);
    /// Get VDPAU GetProcAddress
extern void *GetVDPAUProcAdress(void);
    /// Get VDPAU OSD Output Surface
extern void *GetVDPAUOsdOutputSurface(void);
#endif
#ifdef USE_CUVID
extern void *GetCuvidOsdOutputTexture(unsigned int);
extern int CuvidInitGlx(void);
#endif
extern int DisableOglOsd;

    /// Get OSD size.
extern void VideoGetOsdSize(int *, int *);

    /// Set OSD size.
extern void VideoSetOsdSize(int, int);

    /// Set Osd 3D Mode
extern void VideoSetOsd3DMode(int);


extern int VideoMaxPixmapSize(void);

    /// Set video clock.
extern void VideoSetClock(VideoHwDecoder *, int64_t);

    /// Get video clock.
extern int64_t VideoGetClock(const VideoHwDecoder *);

    /// Set closing flag.
extern void VideoSetClosing(VideoHwDecoder *);

    /// Reset start of frame counter
extern void VideoResetStart(VideoHwDecoder *);

    /// Set trick play speed.
extern void VideoSetTrickSpeed(VideoHwDecoder *, int);

    /// Grab screen.
extern uint8_t *VideoGrab(int *, int *, int *, int);

    /// Grab screen raw.
extern uint8_t *VideoGrabService(int *, int *, int *);

    /// Get decoder statistics.
extern void VideoGetStats(VideoHwDecoder *, int *, int *, int *, int *);

    /// Get video stream size
extern void VideoGetVideoSize(VideoHwDecoder *, int *, int *, int *, int *);

extern void VideoOsdInit(void);		///< Setup osd.
extern void VideoOsdExit(void);		///< Cleanup osd.
#ifdef USE_OPENGLOSD
extern int VideoOsdNeedRestart(void);
#endif

extern void VideoInit(const char *);	///< Setup video module.
extern void VideoExit(void);		///< Cleanup and exit video module.

    /// Poll video input buffers.
extern int VideoPollInput(VideoStream *);

    /// Decode video input buffers.
extern int VideoDecodeInput(VideoStream *);

    /// Get number of input buffers.
extern int VideoGetBuffers(const VideoStream *);

    /// Set DPMS at Blackscreen switch
extern void SetDPMSatBlackScreen(int);

    /// Raise the frontend window
extern int VideoRaiseWindow(void);

/// @}
