#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

#define NB_DEVICE 3
#define MAX_DATA_IN_FRAME 10
#define DATA_SIZE 29
#define MAX_BYTE_IN_FRAME DATA_SIZE*MAX_DATA_IN_FRAME
#define NTP_PACKET_SIZE 48

//parametres IP carte
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xDF, 0xAB};
byte ip[] = {192, 168, 1, 123}; //adresse IP

EthernetServer serveur(80);
SoftwareSerial mySerial(2, 3);

char temp_uart[MAX_BYTE_IN_FRAME] = "";
char temp[DATA_SIZE] = "";
struct Device {
  char id[4];
  char temp[6];
  char hum[6];
  unsigned long last_time;
};

int id_rec = 0;
char temp_rec[6];
char hum_rec[6];

uint16_t trash;

Device all_device[NB_DEVICE];

uint8_t check_sscanf = 0;
int updt_temp = 0;
uint8_t to_update = 0;

void setup() {

  //initialisation des communications séries
  Serial.begin (9600);
  mySerial.begin(9600);

  //initialisation de la communication Ethernet
  Ethernet.init(10);
  delay(1000);
  Ethernet.begin(mac, ip);
  serveur.begin();
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP());

  for (uint8_t i; i < NB_DEVICE; i ++)
  {
    memset(all_device[i].id, '0', sizeof(all_device[i].id) - 1);
    //memset(temp, 0, sizeof(temp));
    sprintf(all_device[i].id, "%d", i);
    //memcpy(all_device[i].id, temp, strlen(temp));

    memcpy(all_device[i].temp, "none", strlen("none"));
    memcpy(all_device[i].hum, "none", strlen("none"));
  }
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
          client.print("      <p> Donnees capteurs : ");
          for (int i = 0; i < NB_DEVICE; i++)
          {
            client.print("      <p> Device ");
            client.print(all_device[i].id);
            client.print(" : ");
            client.print(all_device[i].temp);
            client.print(" C / ");
            client.print(all_device[i].hum);
            client.println(" %");
            client.print("    Derniere donnees :");
            client.print(all_device[i].last_time);
            client.print(" ms");
            client.println("</p>");
          }
          client.print("      <p> temp mcu : ");
          client.print(millis());
          client.println(" ms </p>");
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
    memset(temp_uart, 0, sizeof(temp_uart));
    mySerial.readBytes(temp_uart, MAX_BYTE_IN_FRAME);
    Serial.println(temp_uart);

    char * strToken = strtok(temp_uart, "$");

    uint8_t updt_tmp = 0;

    while ( strToken != NULL )
    {
      memset(temp_rec, 0, sizeof(temp_rec));
      memset(hum_rec, 0, sizeof(hum_rec));

      check_sscanf = sscanf(strToken, "&i=%d&t=%[^&]&h=%s", &id_rec, &temp_rec, &hum_rec);

      if (check_sscanf == 3)
      {
        memcpy(all_device[id_rec].temp, temp_rec, sizeof(temp_rec));
        memcpy(all_device[id_rec].hum, hum_rec, sizeof(hum_rec));
        all_device[id_rec].last_time = millis();
      }
      strToken = strtok( NULL, "$" );
    }
  }
  if (Serial.available())
  {
    memset(temp_uart, 0, sizeof(temp_uart));
    trash = 0;

    mySerial.readBytes(temp_uart, 20);
    check_sscanf = sscanf(temp_uart, "%hu/%hu/%hu-%hu:%hu:%hu", &trash, &trash, &trash, &trash, &trash, &trash);

    if (check_sscanf == 6)
    {
      Serial.write(temp_uart, strlen(temp_uart));
      mySerial.write(temp_uart, strlen(temp_uart));
    }

  }
}
