#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>



void show_fb_fix_info(struct fb_fix_screeninfo info);
void show_fb_var_info(struct fb_var_screeninfo info);
/******************************************************************************
 *
 ******************************************************************************/
int main( int argc, char *argv[] )
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information.\n");
        exit(2);
    }
    show_fb_fix_info(finfo);


	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error: reading variable information.\n");
		exit(3);
	}
    show_fb_var_info(vinfo);
    close(fbfd);
    return 0;
}


void show_fb_fix_info(struct fb_fix_screeninfo info)
{
    printf("fb's fix msg:\n");
    printf("\tid is:%s\n",info.id);
    printf("\tsmem_start is:%d\n",info.smem_start);
    printf("\tsmem_len is:%d\n",info.smem_len);
    printf("\ttype_aux is:%d\n",info.type_aux);
    printf("\tvisual is:%d\n",info.visual);
    printf("\txpanstep is:%d\n",info.xpanstep);
    printf("\typanstep is:%d\n",info.ypanstep);
    printf("\tywrapstep is:%d\n",info.ywrapstep);
    printf("\tline_length is:%d\n",info.line_length);
    printf("\tmmio_start is:%d\n",info.mmio_start);
}

void show_fb_var_info(struct fb_var_screeninfo info)
{
    printf("fb's var msg:\n");
    printf("\txres is:%d\n",info.xres);
    printf("\tyres is:%d\n",info.yres);
    printf("\txres_virtual is:%d\n",info.xres_virtual);
    printf("\tyres_virtual is:%d\n",info.yres_virtual);
    printf("\txoffset is:%d\n",info.xoffset);
    printf("\tyoffset is:%d\n",info.yoffset);
    printf("\tbits_per_pixel is:%d\n",info.bits_per_pixel);
    printf("\tgrayscale is:%d\n",info.grayscale);
    printf("\tnonstd is:%d\n",info.nonstd);
    printf("\tactivate is:%d\n",info.activate);
    printf("\theight is:%d\n",info.height);
    printf("\twidth is:%d\n",info.width);
    printf("\taccel_flags is:%d\n",info.accel_flags);
    printf("\tpixclock is:%d\n",info.pixclock);
    printf("\tleft_margin is:%d\n",info.left_margin);
    printf("\tright_margin is:%d\n",info.right_margin);
    printf("\tupper_margin is:%d\n",info.upper_margin);
    printf("\tlower_margin is:%d\n",info.lower_margin);
    printf("\thsync_len is:%d\n",info.hsync_len);
    printf("\tvsync_len is:%d\n",info.vsync_len);
    printf("\tsync is:%d\n",info.sync);
    printf("\tvmode is:%d\n",info.vmode);
    printf("\trotate is:%d\n",info.rotate);
    printf("\tcolorspace is:%d\n",info.colorspace);
    printf("\tR:%d,G:%d,B:%d \n", info.red.length, info.green.length, info.blue.length );
}