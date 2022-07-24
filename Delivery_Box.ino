#include <Keypad.h>
#include <WiFi.h>
#include <Servo.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "Skye"
#define WIFI_PASSWORD "lywe0934"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "deliverybox2605@gmail.com"
#define AUTHOR_PASSWORD "vfdnprsuiyxwsesr"

#define RECIPIENT_EMAIL "sprihaghosh26@gmail.com"

SMTPSession smtp;
void smtpCallback(SMTP_Status status);

#define ROW_NUM     4  // four rows
#define COLUMN_NUM  4  // four columns
int position = 0;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

Servo servo1;
static const int servoPin = 13;
int sensorPin = 7;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {19, 18, 5, 17};    // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
byte pin_column[COLUMN_NUM] = {16, 4, 0, 2}; // GIOP16, GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup() 
{
  Serial.begin(115200);
  pinMode(sensorPin,INPUT);
  servo1.attach(servoPin);
  openbox();
  initWiFi();
  
  MailClient.networkReconnect(true);

  smtp.debug(1);

  smtp.callback(smtpCallback);

  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "gmail.com";

  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Spriha", RECIPIENT_EMAIL);

  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

    if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

void loop() {

  String password = "";
  int val = digitalRead(sensorPin);
  if (val==0)
  {openbox();
  }
  else if (val==1)
  delay(10000);
  {closebox();
  //password = String(random(1000,10000));
  password = "4321";
  }

  char key = keypad.waitForKey();
  
  if(password != ""){

    int pos = 0;
    int t = 0;
    if(key=='*' || key=='#'){
    pos = 0;
    closebox();
    }

    boolean p;
    while(p = true){
      int c;
      for(c=0;c<4;c++){

         int correctpw = 0;
         for(pos=0; pos<5; pos++){
           if (key==password[pos]){
           correctpw++;}
         }
         
         if(correctpw == 4){ //if password entered is correct
          openbox();
          p = false;
          break;}

         else{
           
           if(c==3){ //for every 3 attempts made, time gets delayed by 10 minutes 
             t = t+600000;
             delay(t);
             p = false;
           }
           else{  //1st and 2nd attempt, gets delayed by 30 seconds
              delay(30000);
              c+=1;           
           }
         }
      }
    }
  }
}

void closebox()
{
  int pos=0;
  for(pos = 0; pos < 90; pos += 1)  // goes from 0 degrees to 90 degrees 
  {                                  // in steps of 1 degree 
    servo1.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  delay(500);
}

void openbox()
{
  int pos = 0;
  for(pos = 90; pos>=1; pos-=1)     // goes from 90 degrees to 0 degrees 
  {                                
    servo1.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
    delay(500);
}    

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}
