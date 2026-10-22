#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Wi-Fi nativo do Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// IP do Servidor
const char* mqtt_server = "";

// Mapeamento de Pinos
#define DHTPIN 15
#define DHTTYPE DHT22
#define LDRPIN 34 
#define LED_PIN 2  

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.print("Comando recebido: ");
  Serial.println(mensagem);

  // No MyMQTT, se você enviar exatamente a palavra "ligar", o LED acende.
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
  Serial.print("\nConectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "VinheriaESP32-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao Broker MQTT do professor!");
      
      // ASSINA o tópico para receber os comandos do celular
      client.subscribe("/TEF/device067/vinheria/cmd");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
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

    if (isnan(umidade) || isnan(temperatura)) {
      Serial.println("Falha na leitura do DHT!");
      return;
    }

    Serial.print("Temp: "); Serial.print(temperatura); Serial.print("°C | ");
    Serial.print("Umid: "); Serial.print(umidade); Serial.print("% | ");
    Serial.print("Luz: "); Serial.println(luminosidade);

    // Envia os dados (Publish) para o servidor
    client.publish("/TEF/device067/vinheria/attrs/t", String(temperatura).c_str());
    client.publish("/TEF/device067/vinheria/attrs/h", String(umidade).c_str());
    client.publish("/TEF/device067/vinheria/attrs/l", String(luminosidade).c_str());
  }
}
