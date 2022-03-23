#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

#define MAX_DATA_IN_FRAME 1
#define DATA_SIZE 25
#define MAX_BYTE_IN_FRAME DATA_SIZE*MAX_DATA_IN_FRAME

//parametres IP carte
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xDF, 0xAB};
byte ip[] = {192, 168, 1, 123}; //adresse IP

EthernetServer serveur(80);
SoftwareSerial mySerial(2, 3);

char temp_uart[MAX_BYTE_IN_FRAME] = "";
char temp[DATA_SIZE] = "";

char id_rec[4] = "";
char temp_rec[6] = "";
char hum_rec[6] = "";

char id_centrale[] = "001";
char temp_centrale[6] = "none";
char hum_centrale[6] = "none";

char id_capture[] = "002";
char temp_capture[6] = "none";
char hum_capture[6] = "none";

uint8_t check_sscanf = 0;

void setup() {

  //initialisation des communications séries
  Serial.begin (9600);
  mySerial.begin(9600);
  
  //initialisation de la communication Ethernet
  Ethernet.init(10);
  delay(1000);
  Ethernet.begin (mac, ip);
  serveur.begin();
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP());
 }
 
void loop() {
  EthernetClient client = serveur.available(); //on écoute le port
  if (client)
  {
    boolean currentLineIsBlank = true;
    String buffer = "";
    while (client.connected() && !mySerial.available()) {
      if (client.available()) {
        char c = client.read(); // Read from the Ethernet shield
        buffer += c; // Add character to string buffer
        // Client sent request, now waiting for response
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 1");          
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("         <head>");
          client.println("            <title>");
          client.println("                   METEO");
          client.println("             </title>");
          client.println("         </head>");
          client.println("         <body>");
          client.println("      <h1>Station meteo</h1>");
          client.print("      <p> Donnees centrale : ");
          client.print(temp_centrale);
          client.print(" C / ");
          client.print(hum_centrale);
          client.println(" % </p>");
          client.print("      <p> Donnees capteur : ");
          client.print(temp_capture);
          client.print(" C / ");
          client.print(hum_capture);
          client.println(" % </p>");
          client.println("         </body>");
          client.println("</html>");
          break;
        }
      }
    }
    client.stop();
  }
  if (mySerial.available())
  {
    mySerial.readBytes(temp_uart, MAX_BYTE_IN_FRAME);
    char * strToken = strtok (temp_uart, "$" );

    while ( strToken != NULL )
    {
      memcpy(temp, strToken, strlen(strToken));

      check_sscanf = sscanf(temp, "&i=%[^&]&t=%[^&]&h=%[^\r]\r\n", &id_rec, &temp_rec, &hum_rec);

      if (check_sscanf == 3)
      {
        if (memcmp(id_rec, id_centrale, sizeof(id_rec)) == 0)
        {
          memcpy(temp_centrale, temp_rec, sizeof(temp_rec));
          memcpy(hum_centrale, hum_rec, sizeof(hum_rec));
        }
        if (memcmp(id_rec, id_capture, sizeof(id_rec)) == 0)
        {
          memcpy(temp_capture, temp_rec, sizeof(temp_rec));
          memcpy(hum_capture, hum_rec, sizeof(hum_rec));
        }
      }
      else
      {
        if (memcmp(id_rec, id_centrale, sizeof(id_rec)) == 0)
        {
          memcpy(temp_centrale, "none", strlen("none"));
          memcpy(hum_centrale, "none", strlen("none"));
        }
        if (memcmp(id_rec, id_capture, sizeof(id_rec)) == 0)
        {
          memcpy(temp_capture, "none", strlen("none"));
          memcpy(hum_capture, "none", strlen("none"));
        }
      }
      strToken = strtok ( NULL, "$" );
    }
  }
}
/*
  //fonction de décodage des infos client
  int decodage(EthernetClient cl) {
  String valeur = ""; //chaîne pour la récupération des valeurs
  char c = lecture(cl); // on lit la première donnée
  Serial.println();
  Serial.print(c);
  while (cl.available()) { //tant qu'on ne lit pas un &
    c = lecture(cl); //on continue de lire
    Serial.print(c);
    if (c == -1) return 0; //renvoie erreur si fin de fichier
  }
  return 1; // on retourne que tout va bien
  }*/
/*
  //fonction de lecture des données client avec attente.
  char lecture(EthernetClient cli) {
  char ch = cli.read();
  delay(1);
  return ch; //on renvoie le caractère lu
  }*/

/*
  //fonction d'affichage de l'entête HTML
  void afficherPage(EthernetClient cl) {
  //File monFichier = SD.open("test.ard", FILE_READ);
  char c = 0;
    uint16_t i = 0;
    while (c != -1) {
    //c = monFichier.read();
      //if (c > 31 || c == 10) { //permet d'éviter des caractères non affichables
     // cl.write(c);
     // }
    c = html_send[i];
    if (c > 31 || c == 10) {
      cl.write(c);
    }
    i++;
    }

  cl.println("HTTP/1.1 200 OK");
  cl.println("Content-Type: text/html");
  cl.println("Connection: close");
  cl.println("<!DOCTYPE html>");
  cl.println("<html>");
  cl.println("         <head>");
  cl.println("            <title>");
  cl.println("                   METEO");
  cl.println("             </title>");
  cl.println("         </head>");
  cl.println("         <body>");
  cl.println("      <h1>Station météo</h1>");
  cl.print("      <p> Données centrale : ");
  cl.print(temp_centrale);
  cl.print(" C / ");
  cl.print(hum_centrale);
  cl.println(" % </p>");
  cl.print("      <p> Données capteur : ");
  cl.print(temp_capture);
  cl.print(" C / ");
  cl.print(hum_capture);
  cl.println(" % </p>");
  cl.println("         </body>");
  cl.println("</html>");
  //monFichier.close();
  }*/
