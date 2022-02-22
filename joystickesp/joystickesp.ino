#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "index.h"  //Fichier header contenant la page web

WebServer server(80);
const char* ssid = "Livebox-4296";//Enter le nom du réseau et le mot de passe
const char* password = "Santony85";
// Cette voide est éxecutée quand on entre l'ip dans la barre d'adresse 
void handleRoot() {
 String s = MAIN_page; //Lire HTML
 server.send(200, "text/html", s); //Envoi de la page
}
void handleADC() {
 analogReadResolution(9);
 int x = analogRead(39);
 int y = analogRead(36);
 int s = analogRead(34);
 String adcValue = "X="+String(x)+"|Y="+String(y);
 Serial.println(adcValue);
 server.send(200, "text/plane", adcValue); //Envoi des valeurs lues
}
//===============================================================
void setup(void){
  Serial.begin(115200);
  Serial.println();
  Serial.println("Début...");
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid, password);
  Serial.println("Connexion à  ");
  Serial.print(ssid);
  //Attendre la connexion du WiFi
  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
      Serial.print("X");
    }  
  //Affiche l'addresse IP lors de la connexion
  Serial.println("");
  Serial.print("Connecté à:");
  Serial.println(ssid);
  Serial.print("L'addresse IP: ");
  Serial.println(WiFi.localIP());  //L'addresse IP de l'esp 
  server.on("/", handleRoot);      //Affiche la page
  server.on("/readADC", handleADC);//Pour mettre à jour la valeur lue
  server.begin();                  //Lancer le serveur
  Serial.println("Le serveur HTTP fonctionne");
}
//===============================================================
void loop(void){
  server.handleClient();
  delay(1);
}
