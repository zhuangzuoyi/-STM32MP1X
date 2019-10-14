// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2017 Google, Inc
 * Written by Simon Glass <sjg@chromium.org>
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <led.h>
#include <dm/uclass-internal.h>
#include "video.h"
#include <mmc.h>
#include <malloc.h>
#include <console.h>

#define BAD_APPLE_START_BLK  0x00001422
short lcd_buf[800][480];
#define BP_SET   0x0000
#define BP_UNSET  0xffff
#define FRAME_MAX 5100

static struct mmc *init_mmc_device(int dev)
{
	struct mmc *mmc;
	mmc = find_mmc_device(dev);
	if (!mmc) {
		printf("no mmc device at slot %x\n", dev);
		return NULL;
	}
	if (mmc_init(mmc))
		return NULL;
	return mmc;
}


struct blk_desc *init_blk_device(struct mmc *mmc)
{
	struct udevice *dev;
	struct blk_desc *desc;
	device_find_first_child(mmc->dev, &dev);
	if (!dev)
		return NULL;
	desc = dev_get_uclass_platdata(dev);
	return desc;
}

struct video_priv *init_video_priv(void)
{
	struct udevice *dev;

	if (uclass_first_device_err(UCLASS_VIDEO, &dev))
		return NULL;

	struct video_priv *priv = dev_get_uclass_priv(dev);
	return priv;
}

//480X800
//128 X 64

void show_badapple(unsigned char *buf,int x,int y)
{
	int index=0;
	unsigned char current_byte=0;

	for(int j=0;j<8;j++)
	{
		for(int i=0;i<128;i++)
		{
			current_byte = buf[index];
			if(current_byte & 0x01)
				lcd_buf[j*8+x][i+y]=BP_SET;
			else
				lcd_buf[j*8+x][i+y]=BP_UNSET;


			if(current_byte & 0x02)
				lcd_buf[j*8+1+x][i+y]=BP_SET;				
			else
				lcd_buf[j*8+1+x][i+y]=BP_UNSET;


			if(current_byte & 0x04)
				lcd_buf[j*8+2+x][i+y]=BP_SET;
			else
				lcd_buf[j*8+2+x][i+y]=BP_UNSET;


			if(current_byte & 0x08)
				lcd_buf[j*8+3+x][i+y]=BP_SET;
			else
				lcd_buf[j*8+3+x][i+y]=BP_UNSET;


			if(current_byte & 0x10)
				lcd_buf[j*8+4+x][i+y]=BP_SET;
			else
				lcd_buf[j*8+4+x][i+y]=BP_UNSET;


			if(current_byte & 0x20)
				lcd_buf[j*8+5+x][i+y]=BP_SET;
			else
				lcd_buf[j*8+5+x][i+y]=BP_UNSET;


			if(current_byte & 0x40)
				lcd_buf[j*8+6+x][i+y]=BP_SET;
			else
				lcd_buf[j*8+6+x][i+y]=BP_UNSET;


			if(current_byte & 0x80)
				lcd_buf[j*8+7+x][i+y]=BP_SET;
			else
				lcd_buf[j*8+7+x][i+y]=BP_UNSET;
			index ++;
		}
	}	
}


void show_img(struct video_priv *priv)
{
//short lcd_buf[800][480];
	for(int i=0;i<priv->ysize;i++)
	{
		for(int j=0;j<priv->xsize;j++)
		{
			lcd_buf[i][j]=0x0000;
		}
	}
	memcpy(priv->fb, lcd_buf, priv->fb_size);
}


int do_badapple(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int curr_device=-1;
	struct mmc *mmc;
	struct blk_desc *desc;
	struct video_priv *priv;
	unsigned char buf[1024];
	int badapple_count =0;
	int *frame_x;
	int *frame_y;


	if (get_mmc_num() > 0)
		curr_device = 0;
	else {
		puts("No MMC device available\n");
		return 1;
	}


	mmc = init_mmc_device(curr_device);
	if (!mmc)
		return CMD_RET_FAILURE;

	desc = init_blk_device(mmc);

	priv = init_video_priv();

	printf("fb's size:%d\n",priv->fb_size);
	printf("fb's xsize:%d\n",priv->xsize);
	printf("fb's ysize:%d\n",priv->ysize);

	show_img(priv);
	//show_img(priv);
 
	badapple_count = rand() % 50;
	frame_x = malloc(4*badapple_count);
	frame_y = malloc(4*badapple_count);

	for(int i=0;i<badapple_count;i++)
	{
		//max 800-64
		frame_x[i] = rand() % (800-64);
		//max 480 - 128
		frame_y[i] = rand()  % (800-128);
	}


	for(int fm=0;fm<FRAME_MAX;fm++)
	{
		blk_dread(desc,BAD_APPLE_START_BLK+fm*2,2,buf);
		for(int index =0;index<badapple_count;index++)
		{
			show_badapple(buf,frame_x[index],frame_y[index]);
		}

		if(fm % 100 ==0)
			printf("*");
		if (ctrlc())
			break;
		memcpy(priv->fb, lcd_buf, priv->fb_size);
		mdelay(1000/30);
	}
	printf("\n");
	return 0;
}



U_BOOT_CMD(
	badapple, 4, 1, do_badapple,
	"play badapple in lcd",
	"play badapple in lcd\n"
	"play badapple in lcd"
);
