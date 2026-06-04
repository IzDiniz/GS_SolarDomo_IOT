// Projeto SolarDomo Fiap - Iago

#include <WiFi.h>
#include <WebServer.h>

// Definição dos pinos dos LEDs com base no seu circuito do Wokwi
const int LED_VERDE = 21;   // Seguro
const int LED_AMARELO = 22; // Alerta
const int LED_VERMELHO = 23; // Crítico

const int POT_PIN = 34; // Pino do potenciômetro de pressão

// Configuração do Wi-Fi simulado do Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

WebServer server(80);

// Variáveis globais para armazenar o estado atualizado do Domo Lunar
String statusDomo = "Seguro";
int ultimaLeituraPressao = 0;

// Função que gera a página HTML que os astronautas vão acessar na rede local
void enviarPaginaHTML() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Guardião de Habitats Extremos</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #121214; color: #e1e1e6; margin-top: 50px; }";
    html += ".card { background: #202024; padding: 30px; border-radius: 10px; display: inline-block; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }";
    html += "h1 { color: #04d361; }";
    html += ".status { font-size: 24px; font-weight: bold; padding: 10px 20px; border-radius: 5px; margin-top: 20px; display: inline-block; }";
    
    // Altera as cores de destaque da página baseando-se no status do Domo
    if (statusDomo == "Seguro") {
        html += ".Seguro { background-color: #12a454; color: white; }";
    } else if (statusDomo == "Em alerta") {
        html += ".Alerta { background-color: #e83f5b; color: white; }";
    } else if (statusDomo == "Crítico") {
        html += ".Critico { background-color: #bb2525; color: white; animation: piscar 1s infinite; }";
        html += "@keyframes piscar { 0% {opacity: 1;} 50% {opacity: 0.4;} 100% {opacity: 1;} }";
    }
    
    html += "</style>";
    // Atualiza a página automaticamente a cada 2 segundos para o astronauta acompanhar em tempo real
    html += "<meta http-equiv='refresh' content='2'>"; 
    html += "</head><body>";
    html += "<div class='card'>";
    html += "<h1>🌌 Monitoramento Atmosférico - Domo Lunar 🌌</h1>";
    html += "<p>Leitura Atual do Potenciômetro (Pressão): <strong>" + String(ultimaLeituraPressao) + "</strong></p>";
    
    if (statusDomo == "Seguro") {
        html += "<div class='status Seguro'>STATUS: SEGURO</div>";
    } else if (statusDomo == "Em alerta") {
        html += "<div class='status Alerta'>STATUS: EM ALERTA ⚠️</div>";
    } else if (statusDomo == "Crítico") {
        html += "<div class='status Critico'>STATUS: CRÍTICO 🔥</div>";
    }
    
    html += "</div></body></html>";
    
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);
    
    // Conexão com o ponto de acesso simulado do Wokwi
    Serial.print("Conectando ao Wi-Fi simulado...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado com sucesso!");
    Serial.print("IP do Servidor do Domo Lunar: http://");
    Serial.println(WiFi.localIP());

    // Configura a rota raiz do Webserver para carregar a página HTML
    server.on("/", enviarPaginaHTML);
    server.begin();
}

void loop() {
    // Processa as requisições HTTP dos astronautas acessando o IP
    server.handleClient();

    // Lê o valor analógico do sensor (0 a 4095)
    ultimaLeituraPressao = analogRead(POT_PIN);
    
    // LÓGICA DE INFERÊNCIA DEMONSTRATIVA (Traduzida do aprendizado do Edge Impulse)
    // Mapeando o comportamento das 3 classes coletadas nos arquivos CSV
    if (ultimaLeituraPressao < 1500) {
        statusDomo = "Seguro";
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, HIGH);
        digitalWrite(LED_VERMELHO, HIGH);
    } 
    else if (ultimaLeituraPressao > 1500 && ultimaLeituraPressao < 2800) {
        statusDomo = "Em alerta";
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, HIGH);
    } 
    else {
        statusDomo = "Crítico";
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARELO, HIGH);
        digitalWrite(LED_VERMELHO, LOW);
    }

    // Exibe os dados de predição idênticos ao formato do Edge Impulse no Monitor Serial
    Serial.printf("Predições TinyML (Simuladas via Regras) -> %s: | Valor: %d\n", statusDomo.c_str(), ultimaLeituraPressao);
    
    delay(100); // Pequeno delay para estabilizar as leituras
}