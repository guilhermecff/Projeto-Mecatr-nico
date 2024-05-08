#include "mbed.h"

Ticker toggle;
// PARA X
DigitalOut StepDriverXY(D11);
DigitalOut direcaoX(D12);
DigitalOut enableX(D13);
InterruptIn fdcX1(D10); // xmax
//AnalogIn EixoXJoyStick(A0);
//PARA Y
DigitalOut direcaoY(D7);
DigitalOut enableY(D8);
InterruptIn fdcY1(D5); // ymax
//AnalogIn EixoYJoyStick(A0);
// PARA Z
/*DigitalOut StepDriverZ(D5);
DigitalOut direcaoZ(D7);
DigitalOut enableZ(D6);
InterruptIn fdcZ1(D4); // zmax*/


Serial pc(USBTX, USBRX); // declara o objeto pc para comunicação serial

int y, ymax, ymin;
int i = 0;
int j; //valor do joystick
bool referenciadoX=false;
bool referenciadoY=false;
bool referenciadoZ=false;

void flip() {
    StepDriverXY = !StepDriverXY;
    //StepDriverZ= !StepDriverZ;
}


void refEixoX(){
    enableX=1;
    direcaoX=0;
    while(!referenciadoX){
        if(fdcX1==0){
            referenciadoX=true;
            break;
        }
        enableX=0;
    }
    if (referenciadoX){
        direcaoX=!direcaoX;
        wait(0.2);
        enableX=1;
    }

}
void refEixoY(){
    enableY=1;
    direcaoY=0;
    while(!referenciadoY){
        if(fdcY1==0){
            referenciadoY=true;
            break;
        }
        enableY=0;
    }
    if (referenciadoY){
        direcaoY=!direcaoY;
        wait(0.2);
        enableY=1;
    }

}
/*void refEixoZ(){
    
    enableZ=1;
    direcaoZ=0;
    while(!referenciadoZ){
        if(fdcX1==0){
            referenciadoZ=true;
            break;
        }
        enableZ=0;
    }
    if (referenciadoX){
        direcaoZ=!direcaoZ;
        wait(0.2);
        enableX=1;
    }

}

*/



int main() {
    wait(1);
    toggle.attach(&flip, 0.001); // intervalo de 1/1000 de segundo
    
    
    enableX = 1;
    enableY = 1;
    //enableZ = 1;
    StepDriverXY = 1;
    //StepDriverZ = 1;
    
    while (1) {
        if (i==0){
            refEixoX();
            refEixoY();
            //refEixoZ();
            i=1;
        }

        
        

    }
}