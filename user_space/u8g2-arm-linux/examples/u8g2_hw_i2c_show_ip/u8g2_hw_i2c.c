#include <stdio.h>
#include "U8g2lib.h"
#include "ifaddrs.h"
#include "arpa/inet.h"

#define MAX_LENGTH 254

int get_ip(char *buf);

int found_colon(int start,char *buf);

int main(void)
{

    u8g2_t u8g2;
    char ipAddr[MAX_LENGTH]={0};
    char ip_buf[15] = {0};
    int start_index=0;
    int colon_index=0;
    int count=1;
    // Initialization    
    u8g2_Setup_ssd1306_i2c_128x64_noname_f( &u8g2, U8G2_R0, u8x8_byte_arm_linux_hw_i2c, u8x8_arm_linux_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    
    //get ip
    get_ip(ipAddr);
    printf("The ips: %s\n", ipAddr);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
    while(1)
    {
        colon_index = found_colon(start_index,ipAddr);
        printf("colon:%d\n",colon_index);
        if(colon_index >0)
        {
            memcpy(ip_buf,&ipAddr[start_index],colon_index-start_index);
            u8g2_DrawStr(&u8g2, 0, 20* count, ip_buf);
            printf("The ip is:%s\n",ip_buf);
            memset(ip_buf,0,sizeof(ip_buf));
            start_index   = colon_index+1;

        }
        else
        {
            if(start_index < strlen(ipAddr))
            {
                memcpy(ip_buf,&ipAddr[start_index],strlen(ipAddr)-start_index);
                u8g2_DrawStr(&u8g2, 0, 20* count, ip_buf);
                printf("The ip is:%s\n",ip_buf);
                memset(ip_buf,0,sizeof(ip_buf));
            }
            break;
        }
            
        count ++;
    }
    //u8g2_DrawStr(&u8g2, 0, 14, ipAddr);
    u8g2_SendBuffer(&u8g2);

    printf("Initialized ...\n");
    
    return 0;
}


int found_colon(int start,char *buf)
{
    int index=0;
    int len = strlen(buf);
    for(int i=start;i<len;i++)
    {
        if(buf[i] ==';')
            return i;
    }
    return -1;
}

int get_ip(char *buf)
{
    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;
    if(getifaddrs(&ifAddrStruct)!=0)
    {
        printf("error\n");
        return -1;
    }

    while (ifAddrStruct != NULL) {
        if (ifAddrStruct->ifa_addr->sa_family == AF_INET) { //if ip4
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strlen(buf) + strlen(addressBuffer) < MAX_LENGTH - 1)
            {
                if (strlen(buf) > 0)
                {
                     strcat(buf, ";");
                }
                strcat(buf, addressBuffer);
            }
            else
            {
                printf("Too many ips!\n");
                break;
            }
        }
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
    freeifaddrs(ifAddrStruct);
    
}