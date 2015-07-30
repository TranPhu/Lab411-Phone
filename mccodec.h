#ifndef _MCCODEC_H_
#define _MCCODEC_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libavcodec/avcodec.h"
// #include "libavcodec/sw"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavutil/opt.h"
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libavutil/imgutils.h"


/*************************************************************************************************************/
#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

// RGB -> YCbCr
#define CRGB2Y(R, G, B) CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16)
#define CRGB2Cb(R, G, B) CLIP((36962 * (B - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)
#define CRGB2Cr(R, G, B) CLIP((46727 * (R - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)

// YCbCr -> RGB
#define CYCbCr2R(Y, Cb, Cr) CLIP( Y + ( 91881 * Cr >> 16 ) - 179 )
#define CYCbCr2G(Y, Cb, Cr) CLIP( Y - (( 22544 * Cb + 46793 * Cr ) >> 16) + 135)
#define CYCbCr2B(Y, Cb, Cr) CLIP( Y + (116129 * Cb >> 16 ) - 226 )
/*************************************************************************************************************/

#define INBUF_SIZE 		4096
#define AUDIO_INBUF_SIZE 	20480
#define AUDIO_REFILL_THRESH 	4096

    AVCodecContext *en_codec,*de_codec;
    uint8_t *outbuf, *picture_buf,*de_picture_buf,*temp_buf,*de_data,*de_temp;
    AVFrame *picture;
    FILE *_file;
    int outbuf_size;
    //decoder
    AVPacket avpkt;
    AVFrame *de_picture,*pFrame;
    int got_picture,de_len;
    AVFrame *RGBFrame;
    struct SwsContext *ConversionContext;
        
void video_encode(unsigned char *data, unsigned int len, unsigned int width, unsigned int hight);
void write_data(FILE *f, uint8_t *outbuf, int len, char end);
AVCodecContext *create_encodec(enum AVCodecID id, unsigned int width, unsigned int hight, unsigned char fps);
AVCodecContext *create_decodec(enum AVCodecID id, unsigned int width, unsigned int hight, unsigned char fps);
AVFrame *create_frame(AVCodecContext *c, uint8_t *frame_buf);
void decode_init();

#endif