#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>

#define BMP "test.bmp"
typedef struct
{	
	uint16_t	bfType;			
	uint32_t	bfSize;			
	uint32_t	biWidth;			 					 
	uint32_t	biHeight;			
	uint16_t	biBitCount;			
}BMP_HEADER;
 
static char *fbp = 0;
static int screan_width = 0;
static int screan_height = 0;
static int bits_per_pixel = 0;
 
void fill_screen(short *fb_men,short color,int pix_size);
//bmp
BMP_HEADER TFTBmpGetHeadInfo(uint8_t *buf)
{
	BMP_HEADER bmpHead;

	bmpHead.bfType = (buf[0] << 8) + buf[1];		 
	bmpHead.bfSize	= (buf[5]<<24) + (buf[4]<<16) + (buf[3]<<8) + buf[2];   
	bmpHead.biWidth = (buf[21]<<24) + (buf[20]<<16) + (buf[19]<<8) + buf[18]; 
	bmpHead.biHeight = (buf[25]<<24) + (buf[24]<<16) + (buf[23]<<8) + buf[22];
	bmpHead.biBitCount = (buf[29] << 8) + buf[28];	

	return bmpHead;
}


int main ( int argc, char *argv[] )
{
	int fbfd = 0;
	
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	int screensize = 0;
	struct fb_bitfield red;
	struct fb_bitfield green;
	struct fb_bitfield blue;
 
	int bmp_fd=0;
	BMP_HEADER bmp_header;
	int len=0;
	ssize_t ret=0;
	char *buf;
	char buffer[54];

	if(argc !=2)
	{
			printf("arg error\n");
			return -1;
	}

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
	printf("id:%s\n",finfo.id);
	printf("smem_len:%d\n",finfo.smem_len);
	printf("type:%d\n",finfo.type);
	
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error: reading variable information.\n");
		exit(3);
	}
 
	printf("R:%d,G:%d,B:%d \n", vinfo.red.length, vinfo.green.length, vinfo.blue.length );
 
	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
	screan_width = vinfo.xres;
	screan_height = vinfo.yres;
	bits_per_pixel = vinfo.bits_per_pixel;
 
	//计算屏幕的总大小（字节）
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("screensize=%d byte\n",screensize);
 
	//对象映射
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}

	short *fb_s;
	fb_s = (short *)fbp;

	//open bmp
	bmp_fd  = open(argv[1], O_RDWR);
	if(bmp_fd <0)
	{
		printf("open file faile\n");
	}
	
	if(read(bmp_fd,buffer,54) <0)
	{
		printf("read file \"%s\" faile\n",argv[1]);
		return -1;	
	}

	bmp_header = TFTBmpGetHeadInfo(buffer);
	printf("%s's msg:\n",argv[1]);
	printf("\ttype:%2x\n",bmp_header.bfType);
	printf("\tsize:%d\n",bmp_header.bfSize);
	printf("\tWidth:%d\n",bmp_header.biWidth);
	printf("\tHeight:%d\n",bmp_header.biHeight);
	printf("\tBitCount:%d\n",bmp_header.biBitCount);

	if(bmp_header.bfType != 0x424d)
	{
		printf("The picture is not bmp file\n");
		return -1;
	}

	if(bmp_header.biBitCount != 24)
	{
		printf("Just support 24bit bmp file\n");
		return -1;
	}
	int perline_size = bmp_header.biWidth * bmp_header.biBitCount /8;
	printf("per line's size:%d\n",perline_size);

	char * readed_data = (char *)malloc(perline_size);
	int i=0;
	int line_count=0;
	int read_size = perline_size;
	int read_count=0;
	int read_per_line=0;
	int buf_offset=0;
	while((ret = read(bmp_fd,readed_data+buf_offset,read_size)) !=0)
	{

			if(ret == -1)
			{
				if(errno == EINTR)
					continue;
				perror("read");
				break;
			}else if(ret !=perline_size )
			{
				read_per_line += ret;
				read_size = perline_size - read_per_line;
				buf_offset = read_per_line;
			}else 
				read_per_line = ret;

			if(read_per_line == perline_size)
			{
				read_size = perline_size;
				buf_offset =0;

				read_per_line = 0;
				if(bits_per_pixel ==16)
				{
					short pixl = 0;
					if(screan_width > bmp_header.biWidth)
					{
						for(i=0;i<bmp_header.biWidth;i++)
						{
							short blue = (readed_data[i*3]>>3) & 0x001f;
							short green = (readed_data[i*3+1]>>2) & 0x003f;
							short red =  (readed_data[i*3+2]>>3) & 0x001f;


							pixl = (red << 11 ) + (green << 5) + blue;
							*fb_s = pixl;
							fb_s ++;
						}
						fb_s += screan_width -bmp_header.biWidth  ;
					}else if(screan_width <= bmp_header.biWidth)
					{
						for(i=0;i<screan_width;i++)
						{
							short blue = (readed_data[i*3]>>3) & 0x001f;
							short green = (readed_data[i*3+1]>>2) & 0x003f;
							short red =  (readed_data[i*3+2]>>3) & 0x001f;


							pixl = (red << 11 ) + (green << 5) + blue;
							*fb_s = pixl;
							fb_s ++;
						}
						
					}
				}
			}
			line_count++;
			if(line_count >= screan_height)
				break;
	}
	free(readed_data);
	munmap(fbp, screensize);
	close(fbfd);
	close(bmp_fd);
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