#include <sys/types.h>   /* for type definitions */
#include <sys/socket.h>  /* for socket API function calls */
#include <netinet/in.h>  /* for address structs */
#include <arpa/inet.h>   /* for sockaddr_in */
#include <stdio.h>       /* for printf() */
#include <stdlib.h>      /* for atoi() */
#include <string.h>      /* for strlen() */
#include <unistd.h>      /* for close() */
#include <assert.h>

#include "mcgtk.h"
#include "mcv4l2.h"
#include "mcsocket.h"
#include "mccodec.h"
#include "mcudp.h"

int main(int argc, char **argv)
{
    /********configure************/
      SEND = 	00;
      RECEV = 	01;
    /*****************************/
    
    io = V4L2_MEMORY_MMAP;
    dev_name = "/dev/video0";
//     fmt.fmt.pix.width = 352;
//     fmt.fmt.pix.height = 288;
    int w, h;
    w = 352;
    h = 288;	
    
    /*create a socket multicast*/
    slen = sizeof(server_other);
    if((server_socket = creat_server_socket()) != 0)
	printf("Creator socket with ID = %i\n",_sock);
    else
	printf("Creator socket False!\n");
    /*******************************/

	open_device();
	init_device(w, h);
	start_capturing();	
	
    avcodec_register_all();
    av_register_all(); 
    
    outbuf_size = 100000;
    outbuf = malloc(outbuf_size);
    temp_buf = malloc(sizeof(uint8_t*)*(w*h*3));
    de_data = malloc(sizeof(uint8_t*)*(w*h*3));
    de_temp = malloc(sizeof(uint8_t*)*(w*h*3));
    _file = fopen("hello.h264", "wb");
    
    picture_buf = malloc((w*h*3)/2); 		/* size for YUV 420 */
    de_picture_buf = malloc((w*h*3)/2); 		/* size for YUV 420 */
    
    en_codec = create_encodec(AV_CODEC_ID_H264,w,h,25);
    de_codec = create_decodec(AV_CODEC_ID_H264,w,h,25);
    picture = create_frame(en_codec,picture_buf);
    de_picture = create_frame(de_codec,de_picture_buf);
    decode_init();
    got_picture = 0;
#ifdef WITH_GTK
	main_frontend(argc, argv);
#endif
	
    uninit_device();
    close_device();
    close(_sock);
    return 0;
}
