~~~cpp
#include "mbed.h"
#include "Map.hpp"

Serial pc(USBTX, USBRX);
Serial bt(D1, D0);
AnalogIn fsrSensor(A5);
DigitalOut ledPin(LED1);
Map map(0, 0.14, 0, 1000);

int main(void)
{   
    char ch;
    int AnalogValue;
    pc.baud(115200);
    bt.baud(115200);
    pc.printf("Hello World! HC-06 bluetooth and FSR on...\n\r");
    ledPin = 0;
    char led = 0;
    
    while(1){
        if(bt.readable()){
            ch=bt.getc();
            if(ch == 'c'){
               led ^= 1;
               ledPin = led;
            }
            pc.printf("%c", ch);

        }
        else if(pc.readable()){
            ch=pc.getc();
            bt.putc(ch);
        }
        AnalogValue = fsrSensor * 1000;
        bt.printf("%d\n", AnalogValue);
        pc.printf("%d\n", AnalogValue);
       wait(0.05);
    }
}
~~~
