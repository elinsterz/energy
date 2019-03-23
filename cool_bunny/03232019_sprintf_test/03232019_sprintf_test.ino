void setup() {
  Serial.begin(115200);
}

void loop() {

    char radiopacket[30];
    int temperature = 25;
    int humidity = 54;

    char str_1[5] = "T =";
    char str_2[5] = "H =";
    
    sprintf(radiopacket, "%s %i | %s %i",str_1,temperature, str_2,humidity);

//
//    char myConcatenation[80];
//    char myCharArray[16]="A variable name";
//    int myInt=5;
//    sprintf(myConcatenation,"%s + %i",myCharArray,myInt);
    Serial.println(radiopacket);
    delay(1000);
}
