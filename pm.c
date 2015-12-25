#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curl/curl.h>
#include <strings.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#define MAX_TIME 85
#define DHT11PIN 7
uint16_t pm1_0;
uint16_t pm2_5;
uint16_t pm10;
int f = 0;
void httppost( );
int main(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
    int uart0_filestream = -1;


    uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {

        printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
    }


    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

	int counter=0;



	for(;;)
	{


        sleep(1);
        counter++;
        
        if(counter<10)
        continue;
        if (uart0_filestream != -1)
        {


            uint8_t rx_buffer[32];
            int rx_length = read(uart0_filestream, (uint8_t *)rx_buffer, 32);

            if (rx_length < 0)
            {
            }
            else if (rx_length == 0)
            {

                //No data waiting
            }
            else
            {
                //Bytes received
                //printf("c=%d\n",c);
                //rx_buffer[rx_length] = '\0';

                uint8_t test;

                pm1_0 = (rx_buffer[4]<<8)+rx_buffer[5];
                pm2_5 = (rx_buffer[6]<<8)+rx_buffer[7];
                pm10 = (rx_buffer[8]<<8)+rx_buffer[9];
                test = rx_buffer[0];

                if(test==66)
                {
					printf("PM1.0: %d\n",pm1_0);
                    printf("PM2.5: %d\n",pm2_5);
					printf("PM10: %d\n",pm10);
					printf("*******************************\n");
                        if(counter>30)
                       {
						    httppost();
						    counter = 0;
					   } 
                     



                }
            }
        }
    }
	close(uart0_filestream);
	return 0;
}

void httppost( )
{
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        if(curl) {
      //  curl_easy_setopt(curl, CURLOPT_URL, "http://iotser.iots.com.tw:3000/update.json");
       //curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.203:3000/update.json");
        curl_easy_setopt(curl, CURLOPT_URL, "https://dora-thingspeak-1221-doraliao.c9users.io/update.json");

        char postdata1[50];
        sprintf(postdata1,"api_key=C6TUTBGFQY20NH63&field%d=%d&field%d=%d&field%d=%d",2,pm1_0,1,pm2_5,4,pm10);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata1);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
            curl_easy_cleanup(curl);
        }

}

