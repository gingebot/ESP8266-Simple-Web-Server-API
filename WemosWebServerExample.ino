#include <ESP8266WiFi.h>
 
const bool DEBUG = false;
const char* ssid = "YourWifiNetWorkHere";
const char* password = "WifiPasswordHere";

// This ESP8266 program is an example of how a message and repeat can be filtered from a simple HTTP Get request.
// It filters a message and an optional repeat value, once the program is running on the ESP it can be tested by going to a browser and entering in an url similar to:
// http://192.168.1.10/message/duncanmakeyourgardengrow/repeat/20/
// The IP address should be replaced with the IP address assigned to the ESP (Is in the serial monitor output

 
WiFiServer server(80);

// An example function of outputting the data filterered from the request
// This could be anthing you want for instance you own function displaying the message on LED display
void MessageDisplayFunction(String Message, int Repeat) {
  if (Message.length() > 0) {
    int count = 1;
    Serial.println("----------------------------------------------------------");
    Serial.println("DISPLAY MESSAGE");    
    do
    {

    Serial.println(Message);
    count ++;
    } while (count <= Repeat);
  Serial.println("----------------------------------------------------------");
  }

}



//This Function is to decode encoded URL strings (spaces, colons, etc)
//Courtesy of http://arduino.stackexchange.com/questions/18007/simple-url-decoding
String UrlDecoder(String message){
  char data[message.length()+1];
  message.toCharArray(data, message.length()+1);
  
  // Create two pointers that point to the start of the data
  char *leader = data;
  char *follower = leader;

  // While we're not at the end of the string (current character not NULL)
  while (*leader) {
      // Check to see if the current character is a %
      if (*leader == '%') {

          // Grab the next two characters and move leader forwards
          leader++;
          char high = *leader;
          leader++;
          char low = *leader;

          // Convert ASCII 0-9A-F to a value 0-15
          if (high > 0x39) high -= 7;
           high &= 0x0f;

        // Same again for the low byte:
          if (low > 0x39) low -= 7;
          low &= 0x0f;

          // Combine the two into a single byte and store in follower:
          *follower = (high << 4) | low;
      } else {
          // All other characters copy verbatim
          *follower = *leader;
     }

      // Move both pointers to the next character:
      leader++;
      follower++;
  }
  // Terminate the new string with a NULL character to trim it off
  *follower = 0;

  message = String(data);
  return message;
}

 
void setup() {
  Serial.begin(115200);
  delay(10);
   
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request  
  String request = client.readStringUntil('\r');

  Serial.println(request);
  client.flush();
  
  String Message = "";
  int RepeatInt = 0;
  // Match the request

  //Check if the url contains the string /message/ , if it does filter the text after /message/ and before the next / to use as the message to display.
  if (request.indexOf("/message") != -1) {
    int MsgStartIndex = request.indexOf("/message/") + 9;
    int MsgEndIndex = request.indexOf("/", MsgStartIndex);
    Message = request.substring(MsgStartIndex, MsgEndIndex);
    
    if (DEBUG == true) {
		Serial.println("found message request");
    	Serial.println(MsgStartIndex);
    	Serial.println(MsgEndIndex);
    	Serial.println(Message);
	}
  } 

  //Check if the url contains the string /repeat/ , if it does filter the value after /repeat/ and before the next / to use as the number of times the message is repeated.
  if (request.indexOf("/repeat/") != -1){
    
    int RptStartIndex = request.indexOf("/repeat/") + 8;
    int RptEndIndex = request.indexOf("/", RptStartIndex);
    String Repeat = request.substring(RptStartIndex, RptEndIndex);
    RepeatInt = Repeat.toInt();

    if (DEBUG == true) {
    	Serial.println("found repeat request");
    	Serial.println(RptStartIndex);
    	Serial.println(RptEndIndex);
    	Serial.println(Repeat);
		Serial.println(RepeatInt);
	}
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  delay(1);

  
  MessageDisplayFunction(UrlDecoder(Message),RepeatInt);
 
}


