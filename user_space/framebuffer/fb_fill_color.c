#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>


 
static char *fbp = 0;
// static int xres = 0;
// static int yres = 0;
// static int bits_per_pixel = 0;
 
#define RED 0xf800
#define GREEN 0x07e0
#define BLUE 0x001f
#define BLACK  0x0000

void fill_screen(short *fb_men,short color,int pix_size);


int main ( int argc, char *argv[] )
{
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	int screensize = 0;

	//打开显示设备
	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd)
	{
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
		printf("Error：reading fixed information.\n");
		exit(2);
	}

	
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error: reading variable information.\n");
		exit(3);
	}

	//对象映射  //void * mmap(void *start, size_t length, int prot , int flags, int fd, off_t offset)
	fbp = (char *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}

	short *fb_s;
	fb_s = (short *)fbp;
	screensize = vinfo.xres * vinfo.yres;
	printf("Fill red\n");
	fill_screen(fb_s,RED,screensize);
	sleep(1);
	printf("Fill green\n");
	// fb_s = (short *)fbp;
	fill_screen(fb_s,GREEN,screensize);
	sleep(1);
	printf("Fill blue\n");
	// fb_s = (short *)fbp;
	fill_screen(fb_s,BLUE,screensize);
	sleep(1);	
	fill_screen(fb_s,BLACK,screensize);
	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}



void fill_screen(short *fb_men,short color,int pix_size)
{	
	for(int i=0;i<pix_size;i++)
	{
		*fb_men = color;
		fb_men ++;
	}
}