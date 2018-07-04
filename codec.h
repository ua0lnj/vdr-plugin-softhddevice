///
///	@file codec.h	@brief Codec module headerfile
///
///	Copyright (c) 2009 - 2013, 2015 by Johns.  All Rights Reserved.
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

/// @addtogroup Codec
/// @{

//----------------------------------------------------------------------------
//	Defines
//----------------------------------------------------------------------------

#define CodecPCM 0x01			///< PCM bit mask
#define CodecMPA 0x02			///< MPA bit mask (planned)
#define CodecAC3 0x04			///< AC-3 bit mask
#define CodecEAC3 0x08			///< E-AC-3 bit mask
#define CodecDTS 0x10			///< DTS bit mask (planned)

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

#ifndef FF_INPUT_BUFFER_PADDING_SIZE
#define FF_INPUT_BUFFER_PADDING_SIZE AV_INPUT_BUFFER_PADDING_SIZE
#endif

enum HWAccelID {
     HWACCEL_NONE = 0,
     HWACCEL_AUTO,
     HWACCEL_VDPAU,
     HWACCEL_DXVA2,
     HWACCEL_VDA,
     HWACCEL_VIDEOTOOLBOX,
     HWACCEL_QSV,
     HWACCEL_VAAPI,
     HWACCEL_CUVID,
};

///
///     Video decoder structure.
///
struct _video_decoder_
{
     VideoHwDecoder *HwDecoder;          ///< video hardware decoder

     int GetFormatDone;                  ///< flag get format called!
     AVCodec *VideoCodec;                ///< video codec
     AVCodecContext *VideoCtx;           ///< video codec context
     int FirstKeyFrame;                  ///< flag first frame
     AVFrame *Frame;                     ///< decoded video frame

     /* hwaccel options */
     enum HWAccelID hwaccel_id;
     char  *hwaccel_device;
     enum AVPixelFormat hwaccel_output_format;

     /* hwaccel context */
     enum HWAccelID active_hwaccel_id;
     void  *hwaccel_ctx;
     void (*hwaccel_uninit)(AVCodecContext *s);
     int  (*hwaccel_get_buffer)(AVCodecContext *s, AVFrame *frame, int flags);
     int  (*hwaccel_retrieve_data)(AVCodecContext *s, AVFrame *frame);
     enum AVPixelFormat hwaccel_pix_fmt;
     enum AVPixelFormat hwaccel_retrieved_pix_fmt;
     AVBufferRef *hw_frames_ctx;

     void *hwdec_priv;
     // For HDR side-data caching
     double cached_hdr_peak;
     // From VO
     struct mp_hwdec_devices *hwdec_devs;
};

//----------------------------------------------------------------------------
//	Typedefs
//----------------------------------------------------------------------------

    /// Video decoder typedef.
typedef struct _video_decoder_ VideoDecoder;

    /// Audio decoder typedef.
typedef struct _audio_decoder_ AudioDecoder;

//----------------------------------------------------------------------------
//	Variables
//----------------------------------------------------------------------------

    /// Flag prefer fast xhannel switch
extern char CodecUsePossibleDefectFrames;

//----------------------------------------------------------------------------
//	Prototypes
//----------------------------------------------------------------------------

    /// Allocate a new video decoder context.
extern VideoDecoder *CodecVideoNewDecoder(VideoHwDecoder *);

    /// Deallocate a video decoder context.
extern void CodecVideoDelDecoder(VideoDecoder *);

    /// Open video codec.
extern void CodecVideoOpen(VideoDecoder *, int);

    /// Close video codec.
extern void CodecVideoClose(VideoDecoder *);

    /// Decode a video packet.
extern void CodecVideoDecode(VideoDecoder *, const AVPacket *);

    /// Flush video buffers.
extern void CodecVideoFlushBuffers(VideoDecoder *);

    /// Allocate a new audio decoder context.
extern AudioDecoder *CodecAudioNewDecoder(void);

    /// Deallocate an audio decoder context.
extern void CodecAudioDelDecoder(AudioDecoder *);

    /// Open audio codec.
extern void CodecAudioOpen(AudioDecoder *, int);

    /// Close audio codec.
extern void CodecAudioClose(AudioDecoder *);

    /// Set audio drift correction.
extern void CodecSetAudioDrift(int);

    /// Set audio pass-through.
extern void CodecSetAudioPassthrough(int);

    /// Set audio downmix.
extern void CodecSetAudioDownmix(int);

    /// Decode an audio packet.
extern void CodecAudioDecode(AudioDecoder *, const AVPacket *);

    /// Flush audio buffers.
extern void CodecAudioFlushBuffers(AudioDecoder *);

    /// Setup and initialize codec module.
extern void CodecInit(void);

    /// Cleanup and exit codec module.
extern void CodecExit(void);

/// @}
