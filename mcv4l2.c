#ifndef _MCV4L2_C_
#define _MCV4L2_C_
#include "mcv4l2.h"
#include "mccodec.h"
#include "mcudp.h"

void errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
#ifdef WITH_V4L2_LIB
	fprintf(stderr, "%s\n",
			v4lconvert_get_error_message(v4lconvert_data));
#endif
	exit(EXIT_FAILURE);
}

int xioctl(int fd, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fd, request, arg);
	} while (r < 0 && EINTR == errno);
	return r;
}

void process_image(unsigned char *p, int len)
{      
  
#ifdef WITH_V4L2_LIB
	if (v4lconvert_convert(v4lconvert_data,
				&src_fmt,
				&fmt,
				p, len,
				dst_buf, fmt.fmt.pix.sizeimage) < 0)
	{
		if (errno != EAGAIN)
			errno_exit("v4l_convert");
		return;
	}
	p = dst_buf;
	len = fmt.fmt.pix.sizeimage;
#endif
// 	 int i, x, y,z,m,n, out_size;

	//compress video	
	/**********************************************************************/
// 	  fflush(stdout);
// 	  //conver RGB to YUV420P
// 	  /*step 1*/
// 	  z=0;
// 	  for(i=0;i<len;i+=3) {
// 	    
// // 		temp_buf[i] =  (0.257 * p[i]) + (0.504 * p[i+1]) + (0.098 * p[i+2]) + 16;
// 		ImageYUV420.Y[z] = CRGB2Y(p[i],p[i+1],p[i+2]);
// // 		temp_buf[i+1] = (0.439 * p[i]) - (0.368 * p[i+1]) - (0.071 * p[i+2]) + 128;
// 		ImageYUV420.U[z] = CRGB2Cb(p[i],p[i+1],p[i+2]);
// // 		temp_buf[i+2] = -(0.148 * p[i]) - (0.291 * p[i+1]) + (0.439 * p[i+2]) + 128;
// 		ImageYUV420.V[z] = CRGB2Cr(p[i],p[i+1],p[i+2]);
// 		z++;
// 	    }
// 	  /*step 2*/  
// 		      /* Y */
// 	  for(y=0;y<en_codec->height;y++) {
// 		for(x=0;x<en_codec->width;x++) {
// 		    picture->data[0][y * picture->linesize[0] + x] = ImageYUV420.Y[y * 352 + x];
// 		    i+=3;
// 		}
// 	    }
// 		  /* Cb and Cr */
// 		 
// 	  for(y=0,m=0;y<en_codec->height/2,m<en_codec->height;y++,m+=2) {
// 		for(x=0,n=0;x<en_codec->width/2,n<en_codec->width;x++,n+=2) {
// 		    picture->data[1][y * picture->linesize[1] + x] = ImageYUV420.U[m * 352 + n];;
// 		}
// 	  }
// 		
// 	  for(y=0,m=1;y<en_codec->height/2,m<en_codec->height;y++,m+=2) {
// 		for(x=0,n=0;x<en_codec->width/2,n<en_codec->width;x++,n+=2) {
// 		    picture->data[2][y * picture->linesize[2] + x] = ImageYUV420.V[m * 352 + n];;
// 		}
// 	  }
// 	  /*step 3*/
// 	  picture->pts++;
// 	  out_size = avcodec_encode_video(en_codec, outbuf, outbuf_size, picture);
// // 	  printf("encode size = %i\n",out_size);
// 	  write_data(_file,outbuf,out_size,0);
	
	/**********************************************************************/

	//send datas to multicast address
// 	send_data(_sock,outbuf,out_size);
	
	/**********************************************************************
	  memset(temp_buf,0,len);
	  memset(p,0,len);
	  avpkt.data = outbuf;
	  avpkt.size = out_size;
	 
	 fflush(stdout);
	 
	 de_len = avcodec_decode_video2(de_codec, de_picture, &got_picture, &avpkt);
	 if (de_len < 0) {
                fprintf(stderr, "Error while decoding frame\n");
//                 exit(1);
            }
         if(avpkt.data)
	 {
	      avpkt.size -= de_len;
	      avpkt.data += de_len;
	 }
// 	 printf("decode size = %i\n",de_len);
	 if(got_picture){
	 got_picture++;
	 
	 sws_scale(ConversionContext, de_picture->data, de_picture->linesize, 0, 
		de_picture->height, RGBFrame->data, RGBFrame->linesize);
	  
	 p=RGBFrame->data[0]; 
	**********************************************************************/

	 
// 	send_datas(client_socket,outbuf,out_size);
#ifdef WITH_GTK
	gdk_draw_rgb_image(drawing_area->window,
			   drawing_area->style->white_gc,
			   0, 0,		/* xpos, ypos */
			   fmt.fmt.pix.width, fmt.fmt.pix.height,
//			   GDK_RGB_DITHER_MAX,
			   GDK_RGB_DITHER_NORMAL,
			   p,
			   fmt.fmt.pix.width * 3);

#endif	   
}

int read_frame(void)
{
	struct v4l2_buffer buf;

	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0) 
	{
		switch (errno) 
		{
		case EAGAIN:
			return 0;

		case EIO:
			/* Could ignore EIO, see spec. */

			/* fall through */

		default:
			errno_exit("VIDIOC_DQBUF");
		}
	}
	assert(buf.index < n_buffers);

	process_image(buffers[buf.index].start, buf.bytesused);

	if (xioctl(fd, VIDIOC_QBUF, &buf) < 0)
		errno_exit("VIDIOC_QBUF");
		
	return 1;
}

int get_frame()
{
	fd_set fds;
	struct timeval tv;
	int r;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	/* Timeout. */
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	r = select(fd + 1, &fds, NULL, NULL, &tv);
	if (r < 0) {
		if (EINTR == errno)
			return 0;

		errno_exit("select");
	}

	if (0 == r) {
		fprintf(stderr, "select timeout\n");
		exit(EXIT_FAILURE);
	}
	return read_frame();
}

void start_capturing(void)
{
	int i;
	enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		break;
	case V4L2_MEMORY_MMAP:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if (xioctl(fd, VIDIOC_QBUF, &buf) < 0)
				errno_exit("VIDIOC_QBUF");
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (xioctl(fd, VIDIOC_STREAMON, &type) < 0)
			errno_exit("VIDIOC_STREAMON");
		break;
	case V4L2_MEMORY_USERPTR:
		break;
	}
}

void uninit_device(void)
{
	int i;

	switch (io) {
	case IO_METHOD_READ:
		break;
	case V4L2_MEMORY_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 ==
			    munmap(buffers[i].start, buffers[i].length))
				errno_exit("munmap");
		break;
	case V4L2_MEMORY_USERPTR:
		break;
	}
	free(buffers);
}


void init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				"memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n",
			dev_name);
		exit(EXIT_FAILURE);
	}

	buffers = calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0)
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL /* start anywhere */ ,
						buf.length,
						PROT_READ | PROT_WRITE
						/* required */ ,
						MAP_SHARED
						/* recommended */ ,
						fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

void init_device(int w, int h)
{
	struct v4l2_capability cap;
	int ret;
	int sizeimage;

	if (xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n",
				dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n",
			dev_name);
		exit(EXIT_FAILURE);
	}

	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = w;
	fmt.fmt.pix.height = h;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
#ifdef WITH_V4L2_LIB
	v4lconvert_data = v4lconvert_create(fd);
	if (v4lconvert_data == NULL)
		errno_exit("v4lconvert_create");
	if (v4lconvert_try_format(v4lconvert_data, &fmt, &src_fmt) != 0)
		errno_exit("v4lconvert_try_format");
	ret = xioctl(fd, VIDIOC_S_FMT, &src_fmt);
	sizeimage = src_fmt.fmt.pix.sizeimage;
	dst_buf = malloc(fmt.fmt.pix.sizeimage);
	printf("raw pixfmt: %c%c%c%c %dx%d\n",
		src_fmt.fmt.pix.pixelformat & 0xff,
	       (src_fmt.fmt.pix.pixelformat >> 8) & 0xff,
	       (src_fmt.fmt.pix.pixelformat >> 16) & 0xff,
	       (src_fmt.fmt.pix.pixelformat >> 24) & 0xff,
		src_fmt.fmt.pix.width, src_fmt.fmt.pix.height);
#else
	ret = xioctl(fd, VIDIOC_S_FMT, &fmt);
	sizeimage = fmt.fmt.pix.sizeimage;
#endif

	if (ret < 0)
		errno_exit("VIDIOC_S_FMT");
// 
//      /* Note VIDIOC_S_FMT may change width and height. */
// 
	printf("pixfmt: %c%c%c%c %dx%d\n",
		fmt.fmt.pix.pixelformat & 0xff,
	       (fmt.fmt.pix.pixelformat >> 8) & 0xff,
	       (fmt.fmt.pix.pixelformat >> 16) & 0xff,
	       (fmt.fmt.pix.pixelformat >> 24) & 0xff,
		fmt.fmt.pix.width, fmt.fmt.pix.height);

		init_mmap();
}

void close_device(void)
{
	close(fd);
}

int open_device(void)
{
	struct stat st;

	if (stat(dev_name, &st) < 0) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	fd = open(dev_name, O_RDWR /* required */  | O_NONBLOCK, 0);
	if (fd < 0) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	return fd;
}

#endif