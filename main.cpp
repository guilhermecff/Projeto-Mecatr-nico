#include "mbed.h"
#include "TextLCD.h"
#include <cstdio>

Ticker toggle;
// PARA X
DigitalOut StepDriverXY(D11);
DigitalOut direcaoX(D12);
DigitalOut enableX(D13);
InterruptIn fdcX1(D10); // xmax
InterruptIn fdcX2(PB_2);
//PARA Y
DigitalOut direcaoY(D7);
DigitalOut enableY(D8);
InterruptIn fdcY1(D5); // ymax
InterruptIn fdcY2(PB_1);
// PARA Z
DigitalOut direcaoZ(PA_11);
DigitalOut enableZ(PA_12);
InterruptIn fdcZ1(D4); // zmax
InterruptIn fdcZ2(PB_15);

DigitalIn BotaoZcima(PC_12);
DigitalIn BotaoZbaixo(PC_10);

DigitalIn saveButton(PC_11); 
DigitalIn voltarButton(PD_2); 

AnalogIn EixoXJoyStick(PA_0);
AnalogIn EixoYJoyStick(PA_1);

I2C i2c_lcd(PB_9, PB_8);
TextLCD_I2C lcd(&i2c_lcd, 0x4e, TextLCD::LCD20x4);

DigitalOut pipeta(PA_13);

// BOTAO DE EMERGENCIA
InterruptIn botaoEmergencia(PC_3);


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
int selecionar = 0;
int mililitros = 1;
int sinalJog = 1;
int lcdref = 0;


int savedPositions[9][4]; // Array to store up to 9 positions [x, y, z]



int savedCount = 0; // This will keep track of how many positions have been saved
int numSaved = 0;



void flip() {
    StepDriverXY = !StepDriverXY;
}

void lcd_show(int state) {
    switch(state) {
        case 0:
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Ola \n");
        lcd.printf("Pressione o \n");
        lcd.printf("botao amarelo \n");
        lcd.printf("para referenciar");
        break;
        
        case 1: 
        lcd.cls();
        lcd.printf("Referenciamento \n");
        lcd.printf("sendo \n");
        lcd.printf("executado... \n");
        break;

        case 2:
        lcd.cls();
        lcd.printf("Referenciamento \n");
        lcd.printf("concluido \n");
        lcd.printf("Deseja selecionar a posicao de pega? \n");
        break;

        case 3:
        lcd.cls();
        lcd.printf("Botao de emergencia  \n");
        lcd.printf("Travamento   \n");
        lcd.printf("Automatico   \n");
        break;

        case 4:
        lcd.cls();
        lcd.printf("Coleta Principal\n");
        lcd.printf(" X:%4d\n", posX);
        lcd.printf(" Y:%4d\n", posY);
        lcd.printf(" Z:%4d\n", posZ);
        // lcd.printf("Pressione 'CNFRM'");
        break;

        case 5:
        lcd.cls();
        lcd.printf("Coleta Outros\n");
        lcd.printf(" X:%4d\n ", posX);
        lcd.printf(" Y:%4d\n ", posY);
        lcd.printf(" Z:%4d\n", posZ);
        lcd.printf(" mililitros:%4d\n", mililitros);
        // lcd.printf("Pressione 'CNFRM'");
        break;

        case 6:
        lcd.cls();
        lcd.printf("Quantos\n");
        lcd.printf("mililitros: %3d\n", mililitros);
        lcd.printf("Aperte Amarelo para confirmar");
        break;

    }
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

void jog() {
    while (true) {
        while (sinalJog == 1) {
            joyX = EixoXJoyStick.read() * 1000;
            joyY = EixoYJoyStick.read() * 1000;
            
            // Lógica para mover o eixo X
            if (joyX < 430 && fdcX1 == 1) {
                direcaoX = 1;
                enableX = 0;
                if (StepDriverXY == 1) {
                    posX += 1;
                }
            } else if (joyX > 550 && fdcX1 == 1) {
                direcaoX = 0;
                enableX = 0;
                if (StepDriverXY == 1) {
                    posX -= 1;
                }
            } else {
                enableX = 1;
            }

            // Lógica para mover o eixo Y
            if (joyY < 430 && fdcY1 == 1) {
                direcaoY = 1;
                enableY = 0;
                if (StepDriverXY == 1) {
                    posY += 1;
                }
            } else if (joyY > 550 && fdcY1 == 1) {
                direcaoY = 0;
                enableY = 0;
                if (StepDriverXY == 1) {
                    posY -= 1;
                }
            } else {
                enableY = 1;
            }

            // Lógica para mover o eixo Z
            if (BotaoZcima == 0 && fdcZ1 == 1) {
                direcaoZ = 0;
                enableZ = 0;
                posZ += 1;
            } else if (BotaoZbaixo == 0 && fdcZ1 == 1) {
                direcaoZ = 1;
                enableZ = 0;
                posZ -= 1;
            } else {
                enableZ = 1;
            }
            
            if (saveButton == 0) {
                sinalJog = 2;
                wait(0.5);
            }
            printf("\r X=%4d \n", posX);
            printf("\r Y=%4d ", posY);
            printf("\r Z=%4d ", posZ);
        }

        while (sinalJog == 2) {
            enableX = 1;
            enableY = 1;
            enableZ = 1;
            if (savedCount == 0) {
                lcd_show(4);
                savedPositions[savedCount][0] = posX;
                savedPositions[savedCount][1] = posY;
                savedPositions[savedCount][2] = posZ;
                for (int a = 0; a < 3; a++) {
                    pc.printf("\rsavedPositions[%i][%i]:%i\n", savedCount, a, savedPositions[savedCount][a]);
                }
                savedCount++;
                sinalJog = 1;
            } else if (savedCount > 0 && savedCount <= 9) {
                selecionar = 0;
                while (selecionar == 0) {
                    lcd_show(6);
                    if (BotaoZcima == 0) {
                        mililitros += 1;
                        wait(0.2);
                    }
                    if (BotaoZbaixo == 0) {
                        mililitros -= 1;
                        wait(0.2);
                    }
                    printf("\r mililitros=%4d \n ", mililitros);
                    if (saveButton == 0) {
                        selecionar = 1;
                    }
                }
                lcd_show(5);
                savedPositions[savedCount][0] = posX;
                savedPositions[savedCount][1] = posY;
                savedPositions[savedCount][2] = posZ;
                savedPositions[savedCount][3] = mililitros; // Supondo que savedPositions tenha 4 colunas
                savedCount++;
                mililitros = 0;
                wait_ms(1000);

                for (int a = 0; a < 4; a++) { // Looping up to 4 to include mililitros
                    pc.printf("\rsavedPositions[%i][%i]:%i\n", savedCount - 1, a, savedPositions[savedCount - 1][a]);
                }
                sinalJog = 1;
            } else {
                sinalJog = 3;
            }
        }

        if (sinalJog == 3) {
            enableX = 1;
            enableY = 1;
            enableZ = 1;
            break;
        }
    }
}


void voltarInicial() {
    // Mover para Z inicial
    while (posZ != 0) {
        if (posZ < 0) {
            direcaoZ = 0;
            enableZ = 0;
            posZ += 1;
        } else if (posZ > 0) {
            direcaoZ = 1;
            enableZ = 0;
            posZ -= 1;
        }
        wait_ms(100);
        
    }
    enableZ = 1;

    // Mover para Y inicial
    while (posY != 0) {
        if (posY < 0) {
            direcaoY = 1;
            enableY = 0;
            posY += 1;
        } else if (posY > 0) {
            direcaoY = 0;
            enableY = 0;
            posY -= 1;
        }
        wait_ms(100);
    }
    enableY = 1;

    // Mover para X inicial
    while (posX != 0) {
        if (posX < 0) {
            direcaoX = 1;
            enableX = 0;
            posX += 1;
        } else if (posX > 0) {
            direcaoX = 0;
            enableX = 0;
            posX -= 1;
        }
        wait_ms(100);
    }
    enableX = 1;
}

void moveToPosition(int targetX, int targetY, int targetZ) {
    // Move to Z position
    while (posZ != targetZ) {
        if (posZ < targetZ) {
            direcaoZ = 0;
            enableZ = 0;
            posZ += 1;
        } else if (posZ > targetZ) {
            direcaoZ = 1;
            enableZ = 0;
            posZ -= 1;
        }
        wait_ms(10);
    }
    enableZ = 1;

    // Move to Y position
    while (posY != targetY) {
        if (posY < targetY) {
            direcaoY = 1;
            enableY = 0;
            posY += 1;
        } else if (posY > targetY) {
            direcaoY = 0;
            enableY = 0;
            posY -= 1;
        }
        wait_ms(10);
    }
    enableY = 1;

    // Move to X position
    while (posX != targetX) {
        if (posX < targetX) {
            direcaoX = 1;
            enableX = 0;
            posX += 1;
        } else if (posX > targetX) {
            direcaoX = 0;
            enableX = 0;
            posX -= 1;
        }
        wait_ms(10);
    }
    enableX = 1;
}


void jogautomatico() {
    for (int j = 1; j < savedCount; j++) {
        int targetX = savedPositions[j][0];
        int targetY = savedPositions[j][1];
        int targetZ = savedPositions[j][2];
        int mililitros = savedPositions[j][3];
        int inicialX = savedPositions[0][0];
        int inicialY = savedPositions[0][1];
        int inicialZ = savedPositions[0][2];

        while (mililitros > 0) {
            // Move to the initial position (first saved position) based on initial positions
            moveToPosition(inicialX,inicialY,inicialZ);
            wait(0.5);  // Simulate delay for picking up liquid

            // Move to the target position
            moveToPosition(targetX, targetY, targetZ);
            wait(0.5);  // Simulate delay for pouring liquid

            mililitros--;
        }
    }
}






int main() {
    wait(1);
    toggle.attach(&flip, 0.001); // Interval of 1/1000 of a second
    pc.baud(9600);
    pc.printf("inicio");
    enableX = 1;
    enableY = 1;
    enableZ = 1;
    StepDriverXY = 1;
    lcd.setBacklight(TextLCD::LightOn);


    while (1) {
        if (i==0){
            lcd_show(0);
            i = 1;
        }
        if(i==1){
            if(saveButton==0){
                pc.printf("Ref");
                i = 2;
                wait_ms(1000);
            }
        }

        
        else if (i == 2) { // Reference positioning
            lcd_show(1);
            refEixoX();
            refEixoY();
            refEixoZ();
            lcd_show(2);

          
            i = 3;
        }
        else if (i == 3){
            if(saveButton==0){
                i = 4;
                wait_ms(1000);
                pc.printf("Guiga");
            }

        }

        else if (i == 4) { // Manual control (jog)
            jog();     
            i = 5;       
        }
        else if (i == 5){
            pc.printf("VoltarInicio\n");
            voltarInicial();
            i = 6;
        }
        else if (i==6){
            jogautomatico();

        }
    }
}