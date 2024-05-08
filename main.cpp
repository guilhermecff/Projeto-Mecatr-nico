#include "mbed.h"


Ticker toggle;
// PARA X
DigitalOut StepDriverXY(D11);
DigitalOut direcaoX(D12);
DigitalOut enableX(D13);
InterruptIn fdcX1(D10); // xmax
//PARA Y
DigitalOut direcaoY(D7);
DigitalOut enableY(D8);
InterruptIn fdcY1(D5); // ymax
// PARA Z
DigitalOut direcaoZ(D14);
DigitalOut enableZ(D15);
InterruptIn fdcZ1(D4); // zmax

DigitalIn BotaoZcima(PC_11);
DigitalIn BotaoZbaixo(PC_10);



AnalogIn EixoXJoyStick(A0);
AnalogIn EixoYJoyStick(A1);






Serial pc(USBTX, USBRX); // declara o objeto pc para comunicação serial

int y, ymax, ymin;
int i = 0;
bool referenciadoX=false;
bool referenciadoY=false;
bool referenciadoZ=false;
int posX=0;
int posY=0;
int posZ=0;
int joyX;
int joyY;

void flip() {
    StepDriverXY = !StepDriverXY;
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

void refEixoZ(){
    
    enableZ=1;
    direcaoZ=0;
    while(!referenciadoZ){
        if(fdcZ1==0){
            referenciadoZ=true;
            break;
        }
        enableZ=0;
    }
    if (referenciadoZ){
        direcaoZ=!direcaoZ;
        wait(0.2);
        enableZ=1;
    }

}


void jog(){
    joyX = EixoXJoyStick.read() * 1000;
    joyY = EixoYJoyStick.read() * 1000;
    
    // PARA X
    
    if (joyX<430 && fdcX1 == 1){
        direcaoX=1;
        enableX=0;
        if (StepDriverXY==1){
            posX +=1;

        }
    }
    else if (joyX>550 && fdcX1 == 1){
        direcaoX=0;
        enableX=0;
        if (StepDriverXY==1){
            posX -= 1;

        }
            
    }
    else{
        enableX=1;
    }

    //PARA Y

    if (joyY<430 && fdcY1 == 1){
        direcaoY=1;
        enableY=0;
        if (StepDriverXY==1){
            posY -= 1;

        }
    }
    else if (joyY>550 && fdcY1 == 1){
        direcaoY=0;
        enableY=0;
        if (StepDriverXY==1){
            posY -= 1;

        }
            
    }
    else{
        enableY=1;
    }
    //PARA Z

    if (BotaoZcima == 0 && fdcZ1 == 1) {
        // Move Z up
        direcaoZ = 0;
        enableZ = 0;
        posZ += 1;


    } else if (BotaoZbaixo == 0 && fdcZ1 == 1) {
        // Move Z down
        direcaoZ = 1;
        enableZ = 0;
        posZ -= 1;
        

    } else {
        // Both buttons not pressed, stop movement
        enableZ = 1;
    }



}





int main() {
    wait(1);
    toggle.attach(&flip, 0.001); // Interval of 1/1000 of a second

    enableX = 1;
    enableY = 1;
    enableZ = 1;
    StepDriverXY = 1;

    while (1) {
        if (i == 0) { // Reference positioning
            refEixoX();
            refEixoY();
            refEixoZ();
            i = 1;
        }

        if (i == 1) { // Manual control (jog)
            jog();
            
        }
    }
}