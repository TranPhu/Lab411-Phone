#ifndef _MCCODEC_C_
#define _MCCODEC_C_
#include "mccodec.h"
#include <stdlib.h>


AVFrame* create_frame(AVCodecContext* c,uint8_t *frame_buf)
{
    AVFrame *frame;
    int size;
        
    frame = avcodec_alloc_frame();
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;
    frame->pts = 0;
    av_image_alloc(frame->data, frame->linesize, c->width, c->height,
                         c->pix_fmt, 32);
    return frame;
}

AVCodecContext* create_encodec(enum AVCodecID id, unsigned int width, unsigned int hight, unsigned char fps)
{
    AVCodecContext *c= NULL;
    AVCodec *codec;
    
    codec = avcodec_find_encoder(id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    
    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = width;
    c->height = hight;
    /* frames per second */
    c->time_base= (AVRational){1,fps};
    c->gop_size = 10; /* emit one intra frame every ten frames */
    c->max_b_frames=1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
      
    if (id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);
    
    /* open it */
    if (avcodec_open2(c, codec,NULL) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }
    
    return c;
}

AVCodecContext* create_decodec(enum AVCodecID id, unsigned int width, unsigned int hight, unsigned char fps)
{
    AVCodecContext *c= NULL;
    AVCodec *codec;
    
    codec = avcodec_find_decoder(id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = width;
    c->height = hight;
    /* frames per second */
    c->time_base= (AVRational){1,fps};
    c->gop_size = 10; /* emit one intra frame every ten frames */
    c->max_b_frames=1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    
    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
        
    if (id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);
    
    /* open it */
    if (avcodec_open2(c, codec,NULL) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }
    
    return c;
}

void write_data(FILE* f, uint8_t* outbuf, int len, char end)
{
    if(end == 0){
// 	printf("encoding frame (size=%5d)\n", len);
	fwrite(outbuf, 1, len, f);
    }
    else if(end == 1){
	/* add sequence end code to have a real mpeg file */
	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x01;
	outbuf[3] = 0xb7;
	fwrite(outbuf, 1, 4, f);
	fclose(f);
	free(outbuf);
    }
}

void decode_init()
{
    av_init_packet(&avpkt);
    pFrame    = avcodec_alloc_frame();
    got_picture = 0;
    
    ConversionContext = sws_getContext(352, 288,
			    AV_PIX_FMT_YUV420P, 352, 288,
			    AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if(ConversionContext == NULL) {
	fprintf(stderr, "Cannot initialize the conversion context!\n");
	exit(1);
      }
      
      RGBFrame = av_frame_alloc();
      RGBFrame->linesize[0] = 352*3;
      RGBFrame->data[0]     = (uint8_t*)malloc( RGBFrame->linesize[0] * 288 );
//     RGBFrame = av_frame_alloc();
//     avpicture_alloc( &RGBFrame, AV_PIX_FMT_RGB24, 352, 288);
//     avpicture_fill((AVPicture *)RGBFrame,NULL, AV_PIX_FMT_RGB24, 352, 288);
}

#endif