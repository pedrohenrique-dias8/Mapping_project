#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define botao 12 //D6

const char* SSID = "LOURENZETTI";
const char* PASSWORD = "jujubademorango";
WiFiClient wifiClient; 

//MQTT Server
const char* BROKER_MQTT = "iot.eclipse.org";
int BROKER_PORT = 1883;


#define ID_MQTT "BCI01"
#define TOPIC_PUBLISH "BCIBotao1"
PubSubClient MQTT(wifiClient);


//funcoes
void mantemConexoes();
void conectaWifi();
void conectaMQTT();
void enviaValores();

void setup() {
  // put your setup code here, to run once:
  pinMode(botao, INPUT_PULLUP);

  Serial.begin(115200);


  conectaWifi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  mantemConexoes();
  enviaValores();
  MQTT.loop();

}

void mantemConexoes(){
  if (!MQTT.connected()){
      conectaMQTT();
    }
  conectaWifi();
  }

void conectaWifi(){

  if (WiFi.status() == WL_CONNECTED){
    return;
    }
  
  Serial.print("Conectando-se a rede:  ");
  Serial.print(SSID);
  Serial.print("  Aguarde!");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    }

    Serial.println();
    Serial.print("Conctado com sucesso na rede: ");
    Serial.print(SSID);
    Serial.print("  IP obtido:  ");
    Serial.print(WiFi.localIP());
  }

void conectaMQQT(){
  while(!MQTT.connected()) {
    Serial.print("Conectando ao Broker MQTT:  ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)){
        Serial.println("Conectado ao Broker com sucesso!");
      }
    else {
        Serial.println("Nao foi possivel se conectar o broker!");
        Serial.println("Tentando novamente em 10 segundos");
        delay(10000);
      }  
    }
  
  }
  
void enviaValores(){
  
  static bool estadoBotao = HIGH;
  static bool estadoBotaoAnt = HIGH;
  static unsigned long debounceBotao;

  estadoBotao = digitalRead(botao);
  if ( (millis() - debounceBotao) > 30 ){
      if (!estadoBotao && estadoBotaoAnt){

        //botao apertado
        MQTT.publish(TOPIC_PUBLISH, "1");
        Serial.println("Botao apertado, pacote enviado!");


        debounceBotao = millis();
        }else if (estadoBotao && !estadoBotaoAnt){

        //botao solto
        MQTT.publish(TOPIC_PUBLISH, "0");
        Serial.println("Botao apertado, pacote enviado!");

        debounceBotao = millis();  
          }
    }
  estadoBotaoAnt = estadoBotao;
  }
