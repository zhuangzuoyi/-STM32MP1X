#include <stdio.h>
#include "U8g2lib.h"
#include "ifaddrs.h"
#include "arpa/inet.h"
#include "curl/curl.h"

#define MAX_LENGTH 254

CURL *curl;
CURLcode res;

int get_ip(char *buf);

int found_colon(int start,char *buf);
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
int str_search(char *src,char *target);

int main(void)
{

    u8g2_t u8g2;

    static char str[10000000];
    // Initialization    
    u8g2_Setup_ssd1306_i2c_128x64_noname_f( &u8g2, U8G2_R0, u8x8_byte_arm_linux_hw_i2c, u8x8_arm_linux_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    
    //get ip



	curl = curl_easy_init();
	
	curl_easy_setopt(curl, CURLOPT_URL, "http://api.bilibili.com/x/relation/stat?vmid=8969156");
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);
	res = curl_easy_perform(curl);
	 
	curl_easy_cleanup(curl);
	printf("%s\n",str);
    int f = str_search(str,"follower");
    printf("The target index is:%d\n",f);
    char left_str[20]={0};
    memcpy(left_str,&str[f+10],strlen(str)-f-9-3);
    printf("The left str is:%s\n",left_str);

    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
    u8g2_DrawStr(&u8g2, 0, 20, "Bili Follower");
    u8g2_DrawStr(&u8g2, 25, 50, left_str);
    u8g2_SendBuffer(&u8g2);

    return 0;
}


size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
if (strlen((char *)stream) + strlen((char *)ptr) > 999999) return 0;
strcat(stream, (char *)ptr);
return size*nmemb;
}

int str_search(char *src,char *target)
{
    char *s=src;
    char first_char = *target;
    while(1)
    {
        char *left = strchr(s,first_char);
        if(left !=NULL)
        {
            s = left+1;
            int ret = memcmp(left,target,strlen(target));
            if(ret ==0)
            {
                return left-src;
            }
            else
            {
                continue;
            }
            
        }
        else
        {
            return -1;
        }
    }
    return -1;
}