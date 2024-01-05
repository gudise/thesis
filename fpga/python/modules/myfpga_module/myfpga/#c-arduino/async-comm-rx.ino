#define BUFFER_WIDTH 32

#define data 2
#define avail 3
#define stored 4

int bin_to_int(char entrada[BUFFER_WIDTH])
{
  int result=0;
  
  for(int i=0; i<BUFFER_WIDTH; i++)
  {
    if(entrada[i]==1)
      result+= 1<<i; 
  }

  return result;
}

char buffer_in[BUFFER_WIDTH];
void setup() {
  // put your setup code here, to run once:
  pinMode(data, INPUT);
  
  pinMode(avail, INPUT);
  
  pinMode(stored, OUTPUT);
  digitalWrite(stored, LOW);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i=0; i<BUFFER_WIDTH; i++)
  {
    while(digitalRead(avail)==0);

    buffer_in[i] = digitalRead(data);

    digitalWrite(stored, HIGH);

    while(digitalRead(avail)==1);

    digitalWrite(stored, LOW);
  }
  Serial.println(bin_to_int(buffer_in));
}
