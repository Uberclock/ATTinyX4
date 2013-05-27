#define I2C_READ   0x01
#define I2C_WRITE  0x00

void i2c_bangbyte(int scl, int sda, byte c) {
  for(int i=7;i>=0;i--) {
    // i2c data write
    digitalWrite(scl, LOW);
    delayMicroseconds(5);
    digitalWrite(sda, (c >> i) & 1);
    delayMicroseconds(5);
    digitalWrite(scl, HIGH);
    delayMicroseconds(10);
  }
}

byte i2c_readbyte(int scl, int sda) {
  byte c = 0;
  pinMode(sda, INPUT);
  digitalWrite(sda, HIGH);
  
  for(int i=7;i>=0;i--) {
    // i2c data read
    digitalWrite(scl, LOW);
    delayMicroseconds(10);
    digitalWrite(scl, HIGH);
    delayMicroseconds(5);
    c &= (digitalRead(sda) << i);
    delayMicroseconds(5);
  }
  
  pinMode(sda, OUTPUT);
}

void i2c_start(int scl, int sda) {
  // i2c setup
  pinMode(sda, OUTPUT);
  digitalWrite(sda, HIGH);
  pinMode(scl, OUTPUT);
  digitalWrite(scl, HIGH);
  delayMicroseconds(10);
  
  // i2c start
  digitalWrite(sda, LOW);
  delayMicroseconds(10);
}

boolean i2c_ack(int scl, int sda) {
  boolean res;
  // i2c ack
  digitalWrite(scl, LOW);
  delayMicroseconds(5);
  pinMode(sda, INPUT);
  digitalWrite(sda, HIGH);
  delayMicroseconds(5);
  digitalWrite(scl, HIGH);
  delayMicroseconds(5);
  res = digitalRead(sda);
  delayMicroseconds(5);
  pinMode(sda, OUTPUT);
  return res;
}

void i2c_repeatedstart(int sda, int scl) {
  // i2c repeated start
  digitalWrite(scl, LOW);
  delayMicroseconds(5);
  digitalWrite(sda, HIGH);
  delayMicroseconds(5);
  digitalWrite(scl, HIGH);
  delayMicroseconds(5);
  digitalWrite(sda, LOW);
  delayMicroseconds(5);
}

void i2c_stop(int sda, int scl) {
  // i2c stop
  digitalWrite(scl, LOW);
  delayMicroseconds(5);
  digitalWrite(sda, LOW);
  delayMicroseconds(5);
  digitalWrite(scl, HIGH);
  delayMicroseconds(5);
  digitalWrite(sda, HIGH);
  delayMicroseconds(5);
}

boolean connection_test(int scl, int sda, int add) {
  i2c_start(scl, sda);
  i2c_bangbyte(scl, sda, add & I2C_READ);
  boolean res = i2c_ack(scl, sda);
  i2c_stop(scl, sda);
  
  return res;
}

int driver_read(int scl, int sda, int add) {
  i2c_start(scl, sda);
  i2c_bangbyte(scl, sda, add & I2C_READ);
  i2c_ack(scl, sda);
  byte config = i2c_readbyte(scl, sda);
  i2c_ack(scl, sda);
  byte data = i2c_readbyte(scl, sda);
  boolean ack = i2c_ack(scl, sda);
  i2c_stop(scl, sda);
  
  if(ack)
    return data;
  else
    return -1;
}

void driver_write(int scl, int sda, int add, byte current) {
  i2c_start(scl, sda);
  i2c_bangbyte(scl, sda, add & I2C_WRITE);
  i2c_ack(scl, sda);
  i2c_bangbyte(scl, sda, 0x78);
  i2c_ack(scl, sda);
  i2c_bangbyte(scl, sda, current);
  i2c_ack(scl, sda);
  i2c_bangbyte(scl, sda, 0x00);
  i2c_ack(scl, sda);
  i2c_stop(scl, sda);
}
