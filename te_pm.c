#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#define MAX_TIME 85
#define DHT11PIN 7
int dht11_val[5]={0,0,0,0,0};
uint16_t pm1_0;
uint16_t pm2_5;
uint16_t pm10;

void dht11_read_val()
{
  uint8_t lststate=HIGH;
  uint8_t counter=0;
  uint8_t j=0,i;
  float farenheit;
  for(i=0;i<5;i++)
     dht11_val[i]=0;
  pinMode(DHT11PIN,OUTPUT);
  digitalWrite(DHT11PIN,LOW);
  delay(18);
  digitalWrite(DHT11PIN,HIGH);
  delayMicroseconds(40);
  pinMode(DHT11PIN,INPUT);
 // printf("digitalRead(DHT11PIN)=%d\n",digitalRead(DHT11PIN));
  for(i=0;i<MAX_TIME;i++)
  {
    counter=0;
    while(digitalRead(DHT11PIN)==lststate){
      counter++;
      delayMicroseconds(1);
      if(counter==255)
        break;
    }
    //printf("i=%d\n",i);
   // printf("digitalRead(DHT11PIN)=%d\n",digitalRead(DHT11PIN));
    lststate=digitalRead(DHT11PIN);
    if(counter==255)
       break;
    // top 3 transistions are ignored
    if((i>=4)&&(i%2==0)){
      dht11_val[j/8]<<=1;
      if(counter>16)
        dht11_val[j/8]|=1;
      j++;
    }
  }
  // verify cheksum and print the verified data
  if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF)))
  {
    farenheit=dht11_val[2]*9./5.+32;
    printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",dht11_val[0],dht11_val[1],dht11_val[2],dht11_val[3],farenheit);
  }
  else
    printf("Invalid Data!!\n");
}

void pm_read()
{
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
					dht11_read_val();
					printf("*******************************\n");
                      
                     



                }
            }
        }
    }
	close(uart0_filestream);
}
int main(void)
{
  printf("Interfacing Temperature and Humidity Sensor (DHT11) and PM2.5 Sensor With Raspberry Pi\n");
  if(wiringPiSetup()==-1)
    exit(1);
  while(1)
  {
     //dht11_read_val();
     pm_read();
     delay(3000);
  }
  return 0;
}
