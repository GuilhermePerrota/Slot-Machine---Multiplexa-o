/*
 PROJETO: Slot Machine com Sistema de Pontuação
 AUTOR: Guilherme Perrota
 DATA: 05/07/2026 
 
 DESCRIÇÃO:
 Slot Machine de 3 dígitos multiplexados usando 7 segmentos (Catodo Comum).
 Utiliza transistores NPN para chaveamento dos comuns e leitura debounced de botão.
 Contém lógica de validação de pontuação e feedback via Serial.
 */

//Definição de pinos
const byte SEGMENTOS[7] = {7, 8, 9, 10, 11, 13, 12}; // A, B, C, D, E, F, G
const byte DISPLAYS[3]  = {6, 5, 4};                 // Dígito 1 (Esq), Dígito 2 (Meio), Dígito 3 (Dir)
const byte PINO_BOTAO   = 3;

//Sistema de exibição de números
const bool NUMEROS[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

// Variáveis de estado e sistema de pontuação
int valorDisplay[3] = {1, 2, 3};
bool travado[3]     = {false, false, false};

byte estadoJogo = 0; // 0: Sorteando 3, 1: Trava D1, 2: Trava D2, 3: Trava D3 & Resultado, 4: Exibindo Pontuação
int pontuacaoAtual = 100; // Pontuação inicial

// Controle de Tempo
unsigned long tempoAnteriorSorteio = 0;
const int velocidadeSorteio        = 500; 

// Controle de Debounce do Botão
bool ultimoEstadoBotao = HIGH;
bool estadoBotao       = HIGH;
unsigned long ultimoTempoClique = 0;
const unsigned long tempoDebounce = 80;


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  // Configura pinos dos segmentos
  for (byte i = 0; i < 7; i++) {
    pinMode(SEGMENTOS[i], OUTPUT);
  }

  // Configura pinos dos displays (Controle dos Transistores)
  for (byte i = 0; i < 3; i++) {
    pinMode(DISPLAYS[i], OUTPUT);
    digitalWrite(DISPLAYS[i], LOW);
  }

  // Configura botão com Pull-Up interno
  pinMode(PINO_BOTAO, INPUT_PULLUP);

  Serial.println(F("=== MÁQUINA CAÇA-NÍQUEL INICIALIZADA ==="));
  Serial.print(F("Pontuação Inicial: "));
  Serial.println(pontuacaoAtual);
}


void loop() {
  lerBotao();
  atualizarSorteio();
  multiplexarDisplays();
}

//////Funções

// Envia os estados dos segmentos para o display ativo
void mostrarDigito(int numero) {
  if (numero < 0 || numero > 9) return;
  for (byte i = 0; i < 7; i++) {
    digitalWrite(SEGMENTOS[i], NUMEROS[numero][i]);
  }
}

// Desliga todos os transistores (apaga os displays)
void apagarDisplays() {
  digitalWrite(DISPLAYS[0], LOW);
  digitalWrite(DISPLAYS[1], LOW);
  digitalWrite(DISPLAYS[2], LOW);
}

// Faz o chaveamento rápido (multiplexação) dos 3 displays
void multiplexarDisplays() {
  for (byte i = 0; i < 3; i++) {
    apagarDisplays();
    mostrarDigito(valorDisplay[i]);
    digitalWrite(DISPLAYS[i], HIGH);
    delayMicroseconds(2500); // 2.5ms por display (~133Hz de refresh rate)
  }
  apagarDisplays();
}

// Atualiza aleatoriamente os números que não estão travados
void atualizarSorteio() {
  if (estadoJogo < 3) {
    if (millis() - tempoAnteriorSorteio >= velocidadeSorteio) {
      tempoAnteriorSorteio = millis();

      for (byte i = 0; i < 3; i++) {
        if (!travado[i]) {
          valorDisplay[i] = random(1, 10);
        }
      }
    }
  }
}

// Trata o clique do botão com debounce
void lerBotao() {
  bool leitura = digitalRead(PINO_BOTAO);

  if (leitura != ultimoEstadoBotao) {
    ultimoTempoClique = millis();
  }

  if ((millis() - ultimoTempoClique) > tempoDebounce) {
    if (leitura != estadoBotao) {
      estadoBotao = leitura;

      // Evento de Clique (Transição HIGH -> LOW)
      if (estadoBotao == LOW) {
        switch (estadoJogo) {
          case 0:
            travado[0] = true;
            estadoJogo = 1;
            break;
          case 1:
            travado[1] = true;
            estadoJogo = 2;
            break;
          case 2:
            travado[2] = true;
            estadoJogo = 3;
            processarResultado(); // Calcula pontos ao travar o último dígito
            break;
          case 4:
            reiniciarRodada();
            break;
        }
      }
    }
  }
  ultimoEstadoBotao = leitura;
}

// Avalia a combinação obtida e ajusta a pontuação
void processarResultado() {
  int d1 = valorDisplay[0];
  int d2 = valorDisplay[1];
  int d3 = valorDisplay[2];

  Serial.print(F("Resultado Final: "));
  Serial.print(d1); Serial.print(F(" - "));
  Serial.print(d2); Serial.print(F(" - "));
  Serial.println(d3);

  // Animação de Par
  if (d1 == d2 && d2 == d3) {
    // 3 IGUAIS (+100 PONTOS)
    pontuacaoAtual += 100;
    Serial.println(F(">>> JACKPOT! +100 Pontos <<<"));
    
    // Pisca a combinação 5 vezes
    for (byte p = 0; p < 5; p++) {
      apagarDisplays();
      delay(100);
      for (int i = 0; i < 20; i++) multiplexarDisplays();
    }
  } 
  else if (d1 == d2 || d2 == d3 || d1 == d3) {
    // 2 IGUAIS -> PAR (+25 PONTOS)
    pontuacaoAtual += 25;
    Serial.println(F(">>> PAR! +25 Pontos <<<"));
    delay(500);
  } 
  else {
    // NENHUM EQUIVALENTE -> AZAR (-10 PONTOS)
    pontuacaoAtual -= 10;
    if (pontuacaoAtual < 0) pontuacaoAtual = 0; // Evita pontuação negativa
    Serial.println(F(">>> Tente Novamente! -10 Pontos <<<"));
    delay(500);
  }

  Serial.print(F("Pontuação Atual: "));
  Serial.println(pontuacaoAtual);

  // Exibe a Pontuação nos displays por 4 segundos
  mostrarPontuacaoPorTempo(4000);
  estadoJogo = 4; // Pronto para a próxima rodada
}

// Decompõe a pontuação atual em 3 dígitos e mostra nos displays pelo tempo definido
void mostrarPontuacaoPorTempo(unsigned long duracaoMs) {
  int tempPontos = pontuacaoAtual;
  if (tempPontos > 999) tempPontos = 999; // Limite de exibição de 3 dígitos

  // Separa a pontuação em centenas, dezenas e unidades
  valorDisplay[0] = tempPontos / 100;
  valorDisplay[1] = (tempPontos / 10) % 10;
  valorDisplay[2] = tempPontos % 10;

  unsigned long inicio = millis();
  while (millis() - inicio < duracaoMs) {
    multiplexarDisplays();
  }
}

// Reinicia o jogo mantendo a pontuação acumulada
void reiniciarRodada() {
  travado[0] = false;
  travado[1] = false;
  travado[2] = false;
  estadoJogo = 0;
  Serial.println(F("\n--- NOVA RODADA INICIADA ---"));
}