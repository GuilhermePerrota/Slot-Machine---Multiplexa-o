# Slot Machine

Projeto de um caça-níquel eletrônico desenvolvido com **Arduino Uno**, utilizando **3 displays de 7 segmentos multiplexados** por transistores NPN e um sistema completo de lógica de jogo com pontuação e animações de resultado.

Este projeto demonstra conceitos fundamentais de engenharia de software e hardware para sistemas embarcados, incluindo multiplexação por divisão de tempo, leitura de entradas com debounce por software e arquitetura baseada em **Máquina de Estados Finos (FSM)**.

---

## Esquemático / Circuito

<img width="798" height="522" alt="image" src="https://github.com/user-attachments/assets/b35dfb39-cee5-42b5-8a06-e3163ff9ff07" />

---

## Tecnologias e Componentes

### **Hardware**
* **Microcontrolador:** Arduino Uno R3
* **Display:** 3x Displays de 7 segmentos (Cátodo Comum)
* **Atuadores de Sinal:** 3x Transistores NPN (ex: 2N2222 ou BC547)
* **Entrada:** 1x Push button (com resistor de pull-up)
* **Passivos:** 
  * 7x Resistores de 220Ω (limitação de corrente dos segmentos)
  * 3x Resistores de 1kΩ (limitação de corrente na base dos transistores)

### **Software**
* **Linguagem:** C / C++ (IDE Arduino)
* **Comunicação:** Serial UART (9600 Baud) para debug e log de partidas

---

## Conceitos de Engenharia Aplicados

### **1. Multiplexação por Divisão de Tempo**
Para controlar 3 displays de 7 segmentos (21 LEDs individuais) usando o menor número de pinos possível, o circuito utiliza a técnica de multiplexação:
* Os segmentos correspondentes (A até G) de todos os displays estão conectados em paralelo aos pinos digitais do Arduino.
* O pino comum de cada display (Cátodo) é acionado individualmente por um transistor NPN.
* O Arduino alterna o acendimento dos displays em uma frequência imperceptível ao olho humano (~133 Hz), criando a ilusão de que todos estão acesos simultaneamente.

### **2. Debouncing via Software**
Chaves mecânicas geram ruídos elétricos (*bouncing*) ao serem pressionadas. O algoritmo implementa uma janela de tempo de debounce baseada em `millis()` para registrar apenas cliques válidos, eliminando leituras falsas sem bloquear a execução do código.

### **3. Máquina de Estados Finos (FSM)**
O fluxo do jogo é gerenciado por estados bem definidos:
* **Estado 0:** Todos os 3 dígitos alternando aleatoriamente.
* **Estado 1:** 1º dígito travado; 2º e 3º alternando.
* **Estado 2:** 1º e 2º dígitos travados; 3º alternando.
* **Estado 3:** Todos travados + Validação da combinação e animação.
* **Estado 4:** Exibição da pontuação final no display até o próximo clique.

---

## Regras do Jogo e Pontuação

Pontuação inicial padrão: **100 pontos**

| Resultado | Condição | Pontuação | Animação no Display |
| :--- | :--- | :--- | :--- |
| **Jackpot!** | 3 números iguais (ex: 7-7-7) | **+100 pontos** | Displays piscam 5 vezes |
| **Par** | 2 números iguais (ex: 4-4-9) | **+25 pontos** | Transição direta |
| **Azar** | Nenhum número igual | **-10 pontos** | Transição direta |

---

## 🔌 Pinagem (Mapeamento de Hardware)

| Componente | Pino do Arduino | Função |
| :--- | :--- | :--- |
| **Segmento A** | D7 | Controle do Led A |
| **Segmento B** | D8 | Controle do Led B |
| **Segmento C** | D9 | Controle do Led C |
| **Segmento D** | D10 | Controle do Led D |
| **Segmento E** | D11 | Controle do Led E |
| **Segmento F** | D13 | Controle do Led F |
| **Segmento G** | D12 | Controle do Led G |
| **Display 1 (Esq)** | D6 | Base Transistor Q1 |
| **Display 2 (Meio)** | D5 | Base Transistor Q2 |
| **Display 3 (Dir)** | D4 | Base Transistor Q3 |
| **Botão** | D3 | Leitura digital (INPUT_PULLUP) |

---

##  Como Rodar o Projeto

1. acesse: https://www.tinkercad.com/things/hJsrjMqw4S1-slot-machine-com-sistema-de-pontuacao?sharecode=j0kjB2xNQNd-7D8eVCP9ApnfVhVUF0iyMi2VX0NhFzM

ou

2. Monte fisicamente
4. Abra o arquivo `Slot Machine com Sistema de Pontuação.ino` na **Arduino IDE**.
5. Conecte a placa Arduino Uno ao computador.
6. Selecione a porta COM e a placa `Arduino Uno`.
7. Compile e faça o upload do código.
8. Abra o **Monitor Serial** (`Ctrl + Shift + M`) para acompanhar os logs das rodadas.

---

## Autor

Desenvolvido por **Guilherme Perrota**  
* Sinta-se à vontade para entrar em contato ou fazer sugestões de melhoria!
