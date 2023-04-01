#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pinBotao1 12 //D6

const char* SSID = "LOURENZETTI";
const char* PASSWORD = "jujubademorango";
WiFiClient wifiClient; 

//MQTT Server
const char* BROKER_MQTT = "test.mosquitto.org";
int BROKER_PORT = 1883;


#define ID_MQTT "Pmpp01"
#define TOPIC_PUBLISH1 "PmppBotao1" //topico que ele publica
#define TOPIC_PUBLISH2 "PmppBotao2" //topico que ele lê
PubSubClient MQTT(wifiClient);



//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void enviaPacote();     //
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(pinBotao1, INPUT_PULLUP);         
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
  MQTT.setCallback(recebePacote);   
}

void loop() {
  mantemConexoes();
  enviaValores();
  MQTT.loop();
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_PUBLISH2);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void enviaValores() {
static bool estadoBotao1 = HIGH;
static bool estadoBotao1Ant = HIGH;
static unsigned long debounceBotao1;

  estadoBotao1 = digitalRead(pinBotao1);
  if (  (millis() - debounceBotao1) > 30 ) {  //Elimina efeito Bouncing
     if (!estadoBotao1 && estadoBotao1Ant) {

        //Botao Apertado     
        MQTT.publish(TOPIC_PUBLISH1, "1");
        Serial.println("Botao1 APERTADO. Payload enviado.");
        
        debounceBotao1 = millis();
     } else if (estadoBotao1 && !estadoBotao1Ant) {

        //Botao Solto
        MQTT.publish(TOPIC_PUBLISH1, "0");
        Serial.println("Botao1 SOLTO. Payload enviado.");
        
        debounceBotao1 = millis();
     }
     
  }
  estadoBotao1Ant = estadoBotao1;
}

void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    if (msg == "0") {
       digitalWrite(LED_BUILTIN, LOW);
    }

    if (msg == "1") {
       digitalWrite(LED_BUILTIN, HIGH);
    }
}
