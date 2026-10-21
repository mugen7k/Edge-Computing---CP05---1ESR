# 🍷 Vinheria Agnello - Monitoramento IoT (Edge Computing - Checkpoint 05)

## Sobre o Projeto
Este projeto foi desenvolvido para atender à demanda de expansão internacional da Vinheria Agnello. O objetivo é criar um sistema IoT de monitoramento ambiental (temperatura, umidade e luminosidade) para a adega, garantindo a qualidade do vinho. 

Os dados coletados pelo ESP32 são enviados via protocolo MQTT para um servidor back-end estruturado no padrão FIWARE (União Europeia). Através do aplicativo **MyMQTT**, é possível ler essas informações em tempo real e enviar comandos de volta para o sistema.

## Equipe - Event Horizon (1ESR)
* Felipe Peres
* Augusto Torin
* Fabricio Mendoza
* Thiago Silva
* Guilherme Gonçalves

---

## Passo 1: Montagem do Circuito (Hardware)

Você pode montar este circuito fisicamente em uma protoboard ou utilizar o simulador [Wokwi](https://wokwi.com/).

### Componentes Necessários:
* 1x Placa ESP32 (ESP32-WROOM-32D)
* 1x Sensor de Temperatura e Umidade (DHT11 físico ou DHT22 no Wokwi)
* 1x Sensor de Luminosidade (LDR / Photoresistor)
* Jumpers para conexão

### Esquema de Ligação (Wiring):
**1. Sensor de Clima (DHT11 / DHT22):**
* **VCC:** Ligar no pino **3V3** do ESP32 (Energia).
* **GND:** Ligar no pino **GND** do ESP32 (Terra).
* **DATA (ou SDA):** Ligar no pino digital **D15** do ESP32 (Sinal de dados).

**2. Sensor de Luz (LDR):**
* **VCC:** Ligar no pino **3V3** do ESP32.
* **GND:** Ligar no pino **GND** do ESP32.
* **AO (Analog Output):** Ligar no pino analógico **D34** do ESP32. 
*(Atenção: Utilize o pino Analógico (AO) e não o Digital (DO), pois precisamos do valor exato da luminosidade e não apenas se está "claro ou escuro").*

---

## Passo 2: Execução do Código (Software)

### 2.1. Preparando o Ambiente
Se estiver usando a **Arduino IDE**, instale as seguintes bibliotecas pelo Gerenciador de Bibliotecas:
1. `DHT sensor library` (por Adafruit)
2. `PubSubClient` (por Nick O'Leary) - *Para a comunicação MQTT.*

*(Se estiver usando o Wokwi, basta adicionar essas mesmas bibliotecas na aba "Library Manager").*

### 2.2. Configurando as Credenciais
No arquivo principal do código (`.ino`), localize e ajuste as variáveis de rede:
* **Wi-Fi Físico:** Insira o nome (`ssid`) e a senha (`password`) da rede que você vai usar na apresentação.
* **Wi-Fi Wokwi:** Deixe o `ssid` como `"Wokwi-GUEST"` e a senha em branco `""`.
* **Servidor MQTT:** O endereço do Broker FIWARE já deve estar configurado como `3.135.234.101`.

### 2.3. Upload e Execução
1. Conecte o ESP32 ao computador (ou inicie a simulação no Wokwi).
2. Faça o upload do código.
3. Abra o **Monitor Serial** (Baud rate: `115200`).
4. Verifique os logs: O sistema deve exibir a mensagem de "WiFi conectado" e, logo após, "Conectado ao Broker MQTT!". As leituras dos sensores começarão a aparecer na tela a cada 5 segundos.

---

## Passo 3: Verificação com o Aplicativo MyMQTT

O último passo é testar a comunicação de duas vias (Leitura e Escrita) usando o seu smartphone.

### 3.1. Conectando ao Servidor (Broker)
1. Baixe o app **MyMQTT** (Android/iOS).
2. Abra o menu e vá em **Settings** (Configurações).
3. Preencha o campo **Broker** com o IP da faculdade: `3.135.234.101` (Porta: `1883`).
4. Volte à tela inicial e clique em **Connect**. O status deve mudar para *Connected*.

### 3.2. Leitura de Dados (Subscribe)
*Como ver a temperatura, umidade e luz da adega na tela do celular:*
1. Vá na aba **Subscribe** do aplicativo.
2. No campo **Topic**, digite: `/TEF/vinheria/attrs/#` *(O símbolo "#" serve para capturar temperatura, umidade e luz ao mesmo tempo).*
3. Clique no botão **Subscribe**.
4. **Teste Prático:** Altere a luz cobrindo o LDR ou sopre o sensor DHT. Você verá os valores mudando instantaneamente na tela do aplicativo!

### 3.3. Escrita de Dados (Publish)
*Como enviar um comando do celular para o servidor:*
1. Vá na aba **Publish** do aplicativo.
2. No campo **Topic**, digite o tópico de comandos: `/TEF/vinheria/cmd`
3. No campo **Message**, digite a instrução, por exemplo: `{"comando": "ligar_alerta"}`
4. Clique em **Publish**. 
5. **Teste Prático:** Se o ESP32 estiver programado para escutar esse tópico, ele processará o comando (ex: acendendo um LED na placa).