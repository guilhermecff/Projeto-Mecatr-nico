#include "mbed.h"
#include "TextLCD.h"
#include <cstdio>

// Ticker
Ticker toggle;

// Controle do Motor de Passo para o Eixo X
DigitalOut StepDriverXY(D11);
DigitalOut direcaoX(D12);
DigitalOut enableX(D13);
InterruptIn fdcX1(D10); // Fim de curso máximo do eixo X
InterruptIn fdcX2(PB_2); // Fim de curso mínimo do eixo X

// Controle do Motor de Passo para o Eixo Y
DigitalOut direcaoY(D7);
DigitalOut enableY(D8);
InterruptIn fdcY1(D5); // Fim de curso máximo do eixo Y
InterruptIn fdcY2(PB_1); // Fim de curso mínimo do eixo Y

// Controle do Motor de Passo para o Eixo Z
DigitalOut direcaoZ(PA_11);
DigitalOut enableZ(PA_12);
InterruptIn fdcZ1(D4); // Fim de curso máximo do eixo Z
InterruptIn fdcZ2(PB_15); // Fim de curso mínimo do eixo Z

// Botões para controle manual do eixo Z
DigitalIn BotaoZcima(PC_12);
DigitalIn BotaoZbaixo(PC_10);

// Botões de controle
DigitalIn saveButton(PC_11); 
DigitalIn voltarButton(PD_2); 

// Entradas analógicas do Joystick
AnalogIn EixoXJoyStick(PA_0);
AnalogIn EixoYJoyStick(PA_1);

// I2C para o LCD
I2C i2c_lcd(PB_9, PB_8);
TextLCD_I2C lcd(&i2c_lcd, 0x4e, TextLCD::LCD20x4);

// Saída digital para controle da pipeta
DigitalOut pipeta(PA_13);

// Botão de emergência
InterruptIn botaoEmergencia(PC_3);

// LED verde de status
DigitalOut LedVerd(PB_3);

// Comunicação serial
Serial pc(USBTX, USBRX);

// Variáveis globais
bool lcdShow10Shown = false;
int y, ymax, ymin;
int i = 0;
bool referenciadoX = false;
bool referenciadoY = false;
bool referenciadoZ = false;
int posX = 0;
int posY = 0;
int posZ = 0;
int joyX;
int joyY;
int selecionar = 0;
int mililitros = 1;
int sinalJog = 1;
int lcdref = 0;

int savedPositions[9][4]; // Array para armazenar até 9 posições [x, y, z]
int savedCount = 0; // Mantém o rastreamento de quantas posições foram salvas
int numSaved = 0;
int lastMililitros = -1; // Variável para armazenar o último valor de mililitros

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
        lcd.printf("X:%4d ", posX);
        lcd.printf("Y:%4d ", posY);
        lcd.printf("Z:%4d", posZ);
        lcd.printf(" mililitros:%4d\n", mililitros);
        lcd.printf(" Posicao:%4d\n", savedCount);

        // lcd.printf("Pressione 'CNFRM'");
        break;

        case 6:
        lcd.cls();
        lcd.printf("Quantos\n");
        lcd.printf("mililitros: %3d\n", mililitros);
        lcd.printf("Aperte Amarelo para confirmar");
        break;

        case 7:
        lcd.cls();
        lcd.printf("Fim da Selecao\n");
        lcd.printf("Aperte Amarelo para pipetagem");
        break;

        case 8:
        lcd.cls();
        lcd.printf("pipetamento Completo\n");
        lcd.printf("Quer pipetar de novo?\n");
        break;

       

        case 10:
        lcd.cls();
        lcd.printf("\n");
        lcd.printf(" Escolha a posicao\n ");
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
        if (voltarButton == 0) { // Verifica se o voltarButton foi pressionado
            break; // Sai do loop principal e termina a função
        }

        while (sinalJog == 1) {
            if (!lcdShow10Shown) {
                lcd_show(10); // Mostra o lcd_show(10) apenas uma vez
                lcdShow10Shown = true;
            }
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
                wait_ms(50);
            }
            printf("\r X=%4d \n", posX);
            printf("\r Y=%4d ", posY);
            printf("\r Z=%4d ", posZ);
            if (sinalJog != 1) {
                lcdShow10Shown = false; // Reseta a flag quando sair do modo 'jog'
            }

            if (voltarButton == 0) { // Verifica se o voltarButton foi pressionado
                break; // Sai do loop sinalJog == 1
            }
        }

        if (voltarButton == 0) { // Verifica se o voltarButton foi pressionado
            break; // Sai do loop principal e termina a função
        }

        while (sinalJog == 2) {
            enableX = 1;
            enableY = 1;
            enableZ = 1;
            if (savedCount == 0) {
                lcd_show(4);
                wait_ms(2000);
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
                    // Atualiza o display apenas se o valor de mililitros mudar
                    if (mililitros != lastMililitros) {
                        lcd_show(6);
                        lastMililitros = mililitros;
                    }
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

                    if (voltarButton == 0) { // Verifica se o voltarButton foi pressionado
                        break; // Sai do loop selecionar
                    }
                }

                if (voltarButton == 0) { // Verifica se o voltarButton foi pressionado
                    break; // Sai do loop sinalJog == 2
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
                lastMililitros = -1; // Reseta para garantir atualização correta na próxima vez
            } else {
                sinalJog = 3;
            }
        }

        if (voltarButton == 0) { // Verifica se o voltarButton foi pressionado
            break; // Sai do loop principal e termina a função
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
        int fakeml = 0; // Iniciar com 0 e incrementar até mililitros
        
        lcd.cls();
        lcd.printf("Pipetamento\n");
        lcd.printf("Numero Atual:%4d\n ", j);
        lcd.printf("mililitros:%4d\n", mililitros);

        while (fakeml < mililitros) {
            if (voltarButton == 0) { // Verifica se o voltarButton foi pressionado
                return; // Sai da função jogautomatico
            }
            
            // Move to the initial position (first saved position) based on initial positions
            moveToPosition(inicialX, inicialY, inicialZ);
            pipeta = !pipeta;
            wait_ms(50);  // Simulate delay for picking up liquid
            pipeta = !pipeta;
            wait_ms(3000);

            // Move to the target position
            moveToPosition(targetX, targetY, targetZ);
            pipeta = !pipeta;
            wait_ms(50);  // Simulate delay for picking up liquid
            pipeta = !pipeta;
            wait_ms(3000);

            fakeml++;

            // Atualiza o LCD com os mililitros restantes
            lcd.locate(0, 2);
            lcd.printf("mililitros restantes:%4d\n", mililitros - fakeml);
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
            refEixoZ();
            refEixoX();
            refEixoY();
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
            lcd_show(7);     
            i = 5;       
        }
        else if (i == 5){
            if(saveButton==0){
                i = 6;
                wait_ms(1000);
                pc.printf("Guiga");
            }

        }
        else if (i == 6){
            pc.printf("VoltarInicio\n");
            voltarInicial();
            i = 7;
        }
        else if (i==7){
            jogautomatico();
            lcd_show(8);
            i = 8;
            LedVerd = 1;

        }
        else if (i==8){ 
            if(saveButton==0){
                i = 7;
            }
            LedVerd = 1;
        }
    }
}