#define BUFFER_WIDTH 32

#define data 2
#define avail 3
#define stored 4

void int_to_bin(int entrada, char salida[BUFFER_WIDTH])
{
  for(int i=0; i<BUFFER_WIDTH; i++)
  {
    if(entrada%2==1)
      salida[i]=1;
    else
      salida[i]=0;
      
    entrada = entrada/2;  
  }
}

char buffer_out[BUFFER_WIDTH];
void setup() {
  // put your setup code here, to run once:
  pinMode(data, OUTPUT);
  digitalWrite(data, LOW);
  
  pinMode(avail, OUTPUT);
  digitalWrite(avail, LOW);
  
  pinMode(stored, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.read()!='\n');

  for(int i=0; i<1000; i++)
  {
    int_to_bin(i, buffer_out);
    for(int j=0; j<BUFFER_WIDTH; j++)
    {
      digitalWrite(data, buffer_out[j]);

      digitalWrite(avail, HIGH);
      while(digitalRead(stored)==0);
      digitalWrite(avail, LOW);
      while(digitalRead(stored)==1);
    }
  }
}