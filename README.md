# Projeto-Mecatr-nico
# Sistema de Pipetação Automática

Este repositório contém o código-fonte e a documentação para o Sistema de Pipetação Automática, um projeto inovador desenvolvido para automatizar o processo de manipulação de líquidos em ambientes laboratoriais. Utilizando tecnologia de ponta em controle de movimento e precisão volumétrica, este sistema é capaz de melhorar significativamente a eficiência e a precisão em experimentos que requerem pipetação.

## Visão Geral

O Sistema de Pipetação Automática foi projetado com um foco em robustez e precisão. Integrando hardware mecânico customizado e software em C++, o sistema automatiza a transferência de líquidos entre recipientes com controle preciso sobre volume e posicionamento. Este projeto foi desenvolvido utilizando o Keil Studio para a programação dos componentes microcontrolados, com a parte mecânica construída e testada nos laboratórios de engenharia da faculdade.

### Componentes do Sistema

- **Mesa Mecânica:** Estrutura que suporta os movimentos nos eixos X, Y e Z.
- **Controladores de Movimento:** Motores e drivers responsáveis pelo posicionamento preciso da pipeta.
- **Sensores de Posição:** Utilizados para garantir a precisão absoluta dos movimentos da pipeta.
- **Interface de Usuário:** Painel para iniciar operações, calibrar o sistema e configurar volumes de pipetação.

## Tecnologias Utilizadas

- **C++:** Linguagem de programação utilizada para desenvolvimento do firmware.
- **Keil Studio:** Ambiente de desenvolvimento integrado (IDE) para programação de sistemas embarcados.
- **Git:** Sistema de controle de versão para gerenciamento de código-fonte.

## Pré-requisitos

Antes de iniciar, certifique-se de que os seguintes requisitos sejam atendidos:

- Keil Studio v5.30 ou superior.
- Hardware compatível com as especificações de design.
- Acesso administrativo para instalação de drivers de hardware.

## Configuração do Ambiente

### Instalação do Software

1. Instale o Keil Studio seguindo as instruções disponíveis em [Keil - Download](https://www.keil.com/download).
2. Clone o repositório do projeto usando Git:
   ```bash
   git clone https://github.com/seu-usuario/seu-projeto.git
