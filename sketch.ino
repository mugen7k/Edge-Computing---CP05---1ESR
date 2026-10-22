#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Credenciais de rede
const char* ssid = "NOME_DA_SUA_REDE_WIFI";
const char* password = "SENHA_DA_SUA_REDE_WIFI";
const char* mqtt_server = "";

// Mapeamento de Pinos
#define DHTPIN 15
#define DHTTYPE DHT11   // Configurado para o componente físico DHT11
#define LDRPIN 34       // Ligado ao pino AO (Analog Output) do módulo LDR
#define LED_PIN 2       // LED interno do ESP32 para testar os comandos

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

// Função de Callback para receber os comandos do MyMQTT (Escrita)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.print("Comando recebido: ");
  Serial.println(mensagem);

  // Comando para acender ou apagar o LED físico da placa ESP32
  if (mensagem == "ligar") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Ação: Alerta LIGADO!");
  } 
  else if (mensagem == "desligar") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Ação: Alerta DESLIGADO!");
  }
}

void setup_wifi() {
  delay(10);
  Serial.print("\nA ligar à rede ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi ligado. Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("A tentar a ligação MQTT...");
    String clientId = "VinheriaESP32-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Ligado ao Broker MQTT do professor!");
      
      // Assina o tópico para receber os comandos do celular
      client.subscribe("/TEF/vinheria/cmd");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" A tentar novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 

  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
  
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  unsigned long now = millis();
  if (now - lastMsg > 5000) { 
    lastMsg = now;

    float umidade = dht.readHumidity();
    float temperatura = dht.readTemperature();
    int luminosidade = analogRead(LDRPIN);

    // Verificação de falhas no sensor físico
    if (isnan(umidade) || isnan(temperatura)) {
      Serial.println("Falha na leitura do sensor DHT11!");
      return;
    }

    Serial.print("Temp: "); Serial.print(temperatura); Serial.print("°C | ");
    Serial.print("Umid: "); Serial.print(umidade); Serial.print("% | ");
    Serial.print("Luz: "); Serial.println(luminosidade);

    // Publicação dos dados para o servidor (Leitura)
    client.publish("/TEF/vinheria/attrs/t", String(temperatura).c_str());
    client.publish("/TEF/vinheria/attrs/h", String(umidade).c_str());
    client.publish("/TEF/vinheria/attrs/l", String(luminosidade).c_str());
  }
}
