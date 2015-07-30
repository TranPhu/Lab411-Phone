#ifndef _MCV4L2_H_
#define _MCV4L2_H_

// define library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>		/* getopt_long() */
#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "stdint.h"

#include <asm/types.h>		/* for videodev2.h */
#include <linux/videodev2.h>
#include <gtk/gtk.h>
#include "mcgtk.h"

/**************************************************************/
#define WITH_V4L2_LIB	 	1		/* v4l library */
#define SVV_VERSION 		"0.1"
#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define IO_METHOD_READ 7	/* !! must be != V4L2_MEMORY_MMAP / USERPTR */
#define NFRAMES 30

typedef struct _ImageYUV{
      uint8_t Y[352*288];
      uint8_t U[352*288];
      uint8_t V[352*288];
    };
    
    struct _ImageYUV ImageYUV420; 
/**************************************************************/

#ifdef WITH_V4L2_LIB
#include "libv4lconvert.h"
struct v4lconvert_data *v4lconvert_data;
struct v4l2_format src_fmt;	/* raw format */
unsigned char *dst_buf;
#endif

struct v4l2_format fmt;		/* gtk pormat */
char *dev_name;// = "/dev/video0";
int io;// = V4L2_MEMORY_MMAP;
int fd;
struct buffer *buffers;
int n_buffers;
int grab, info, display_time;
struct buffer 
{
	void *start;
	size_t length;
};
struct timeval cur_time;

unsigned char *data_send;
/***********************funtion*********************************/
void errno_exit(const char *s);
int xioctl(int fd, int request, void *arg);
void process_image(unsigned char *p, int len);
int read_frame(void);
int get_frame();
void stop_capturing(void);
void start_capturing(void);
void uninit_device(void);
void init_mmap(void);
void init_device(int w, int h);
void close_device(void);
int open_device(void);
void _display(unsigned char* data,GtkWidget *area);
/*******************end funtion*********************************/


#endif