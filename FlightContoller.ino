//指令定义
#define READ_REG  0x00
#define WRITE_REG 0x20
#define RD_RX_PLOAD 0x61
#define WR_TX_PLOAD 0xA0
#define FLUSH_TX  0xE1
#define FLUSH_RX  0xE2
#define REUSE_TX_PL 0xE3
#define NOP 0xFF
//中断标志位定义
#define RX_DR 0x40
#define TX_DS 0x20
#define MAX_RT  0x10
//寄存器定义
#define CONFIG  0x00
#define EN_AA 0x01
#define EN_RXADDR 0x02
#define SETUP_AW  0x03
#define SETUP_RETR  0x04
#define RF_CH 0x05
#define RF_SETUP  0x06
#define STATUS  0x07
#define OBSERVE_TX  0x08
#define CD  0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17
//定义引脚
//#define IRQ 
#define CE  7
#define CSN 8
#define MISO_2   9

void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(CE,OUTPUT);
  pinMode(CSN,OUTPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(MISO_2,INPUT_PULLUP);
  pinMode(SCK,OUTPUT);

  init_nrf24l01();
  SPI_Reg_Write(CONFIG,0x02);
  //SPI_Reg_Write();
  Serial.println(int(0x6e));
  delay(500);
}

void loop() 
{
  byte regValue_nrf;

  regValue_nrf=SPI_Read_2(CONFIG);
  Serial.println((byte)regValue_nrf);
  delay(100);
}

void init_nrf24l01()
{
  digitalWrite(CE,0);
  digitalWrite(CSN,1);

  init_TX_Mode();
}

void SPI_Write(byte Byte)
{
  for(int i = 0; i < 8; i++)
  {
     if(Byte&0x80)
     {
      digitalWrite(MOSI,1);
     }
     else
     {
      digitalWrite(MOSI,0);
     }
     digitalWrite(SCK,1);
     Byte <<= 1;
     digitalWrite(SCK,0);
  }
}

byte SPI_Read(byte address)
{
  byte dat = 0;
  SPI_Write(address);
  for(int i=0;i<8;i++)
  {
    dat <<= 1;
    digitalWrite(SCK,1);
    delay(1);
    if(digitalRead(MISO_2))
    {
        dat|=1;
    }
    else
    {
        dat|=0;
    }
    digitalWrite(SCK,0);
    delay(1);
  }

  return dat;
}

void SPI_Reg_Write(byte address, byte value)
{
  digitalWrite(CSN,0);
  SPI_Write(byte(address+WRITE_REG));
  SPI_Write(value);
  digitalWrite(CSN,1);
}

byte SPI_Reg_Read(byte address)
{
  byte outputValue;
  digitalWrite(CSN,0);
  outputValue = SPI_Read(address);
  digitalWrite(CSN,1);

  return outputValue;
}

void init_TX_Mode()
{
  
}

byte nRF24L01_SPI_RW(byte dat)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        if(dat&0x80)
        {
            digitalWrite(MOSI,1);
        }
        else
        {
            digitalWrite(MOSI,0);
        }
        dat=(dat<<1);
        digitalWrite(SCK,1);
        if(digitalRead(MISO_2))
        {
            dat|=1;
        }
        else
        {
            dat|=0;
        }
        digitalWrite(SCK,0); 
    }
    
    return(dat);
}

byte SPI_Read_2(byte reg)
{
  byte reg_val;

  digitalWrite(CSN,0);
  nRF24L01_SPI_RW(reg);
  reg_val = nRF24L01_SPI_RW(0);

  digitalWrite(CSN,0);

  return(reg_val);
}
