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
//定义状态寄存器的相关位
#define RX_DR 0x40
#define TX_DS 0x20
#define MAX_RT 0x10

byte RX_Data_Package[4]= {0x00,0,0,0};
byte TX_Data_Package[4]= {0x01,0x02,0x03,0x04};
byte TX_RX_Address[5] = {0x34,0x43,0x10,0x10,0x01};
byte outPut_Address[5];

void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(CE,OUTPUT);
  pinMode(CSN,OUTPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(MISO,INPUT_PULLUP);
  pinMode(SCK,OUTPUT);

  init_nrf24l01();
  Serial.println("-----------------初始化已完成-----------------");
}

void loop() 
{
  Serial.println(RX_Data_Package[0]);
  Reset_RX_Data();
  Serial.println(RX_Data_Package[0]);
}

void init_nrf24l01()
{
  digitalWrite(CE,0);
  digitalWrite(CSN,1);

  init_RX_Mode();
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
    if(digitalRead(MISO))
    {
        dat|=1;
    }
    else
    {
        dat|=0;
    }
    digitalWrite(SCK,0);
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

void Write_TX_Data(byte *TX_Data)
{
  digitalWrite(CSN,0);
  SPI_Write(WR_TX_PLOAD);
  for(int j = 0; j < 4; j++)
  {
    for(int i = 0; i < 8; i++)
    {
       if(TX_Data[j]&0x80)
       {
        digitalWrite(MOSI,1);
       }
       else
       {
        digitalWrite(MOSI,0);
       }
       digitalWrite(SCK,1);
       TX_Data[j] <<= 1;
       digitalWrite(SCK,0);
    }
  }
  digitalWrite(CSN,1);
}

void Read_RX_Data(byte *RX_Data)
{
  digitalWrite(CSN,0);
  SPI_Write(RD_RX_PLOAD);
  for(int j = 0; j < 4; j++)
  {
    for(int i=0;i<8;i++)
      {
        RX_Data[j] <<= 1;
        digitalWrite(SCK,1);
        if(digitalRead(MISO))
        {
            RX_Data[j]|=1;
        }
        else
        {
            RX_Data[j]|=0;
        }
        digitalWrite(SCK,0);
      }
  }
  digitalWrite(CSN,1);
}

void Send_Reset_TX_Data()
{
  byte Reg_Status;

  Reg_Status = SPI_Reg_Read(STATUS);
  Serial.println(Reg_Status);
  if(Reg_Status & TX_DS)
  {
    SPI_Write(FLUSH_TX);
    Write_TX_Data(TX_Data_Package);
  }
  if(Reg_Status & MAX_RT)
  {
    SPI_Write(FLUSH_TX);
    Write_TX_Data(TX_Data_Package);
  }

  SPI_Reg_Write(STATUS,Reg_Status);
  delay(10);
}

void Reset_RX_Data()
{
  byte Reg_Status;
  byte Reg_Status2;

  Reg_Status = SPI_Reg_Read(STATUS);
  Serial.println(byte(Reg_Status));
  delay(100);
  Serial.print("接受中断位：");
  Serial.println(Reg_Status & RX_DR);
  if(Reg_Status & RX_DR)
  {
    Read_RX_Data(RX_Data_Package);
    SPI_Write(FLUSH_RX);
  }

  SPI_Reg_Write(STATUS,0x40);
  Reg_Status2 = SPI_Reg_Read(STATUS);
  Serial.print("Reg_Status2:");
  Serial.println(byte(Reg_Status2));
}

void Write_AddressReg(byte Reg, byte *Address)
{
  digitalWrite(CSN,0);
  SPI_Write(byte(Reg+WRITE_REG));
  for(int j = 0; j <5; j++)
  {
    for(int i = 0; i < 8; i++)
    {
       if(Address[j]&0x80)
       {
        digitalWrite(MOSI,1);
       }
       else
       {
        digitalWrite(MOSI,0);
       }
       digitalWrite(SCK,1);
       Address[j] <<= 1;
       digitalWrite(SCK,0);
    }
  }
  digitalWrite(CSN,1);
}

void Read_AddressReg(byte Reg, byte *outPut)
{
  digitalWrite(CSN,0);
  SPI_Write(byte(Reg+WRITE_REG));
  for(int j = 0; j <5; j++)
  {
    for(int i = 0; i < 8; i++)
    {
      outPut[j] <<= 1;
      digitalWrite(SCK,1);
      if(digitalRead(MISO))
      {
          outPut[j]|=1;
      }
      else
      {
          outPut[j]|=0;
      }
        digitalWrite(SCK,0);
    }
  }
  digitalWrite(CSN,1);
}

void init_TX_Mode()
{
  byte tempValue;
  digitalWrite(CE, 0);
  Write_AddressReg(TX_ADDR, TX_RX_Address);
  Read_AddressReg(TX_ADDR, outPut_Address);
  Serial.print("TX地址寄存器为：");
  Serial.println(outPut_Address[0]);
  Write_AddressReg(RX_ADDR_P0, TX_RX_Address);
  Read_AddressReg(RX_ADDR_P0, outPut_Address);
  Serial.print("发送端RX地址寄存器为：");
  Serial.println(outPut_Address[0]);
  SPI_Reg_Write(EN_AA, 0x01);
  tempValue = SPI_Reg_Read(EN_AA);
  Serial.print("EN_AA寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(EN_RXADDR, 0x01);
  tempValue = SPI_Reg_Read(EN_RXADDR);
  Serial.print("EN_RXADDR寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(CONFIG, 0x0e);
  tempValue = SPI_Reg_Read(CONFIG);
  Serial.print("CONFIG寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(RF_CH, 40);
  tempValue = SPI_Reg_Read(RF_CH);
  Serial.print("RF_CH寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(RF_SETUP, 0x07);
  tempValue = SPI_Reg_Read(RF_SETUP);
  Serial.print("RF_SETUP寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(SETUP_RETR, 0x1a);
  tempValue = SPI_Reg_Read(SETUP_RETR);
  Serial.print("SETUP_RETR寄存器为：");
  Serial.println(tempValue);
  Write_TX_Data(TX_Data_Package);
  digitalWrite(CE, 1);
}

void init_RX_Mode()
{
  byte tempValue;
  digitalWrite(CE, 0);
  Write_AddressReg(RX_ADDR_P0, TX_RX_Address);
  Read_AddressReg(RX_ADDR_P0, outPut_Address);
  Serial.print("TX地址寄存器为：");
  Serial.println(outPut_Address[0]);
  SPI_Reg_Write(EN_AA, 0x01);
  tempValue = SPI_Reg_Read(EN_AA);
  Serial.print("EN_AA寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(EN_RXADDR, 0x01);
  tempValue = SPI_Reg_Read(EN_RXADDR);
  Serial.print("EN_RXADDR寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(RF_CH, 40);
  tempValue = SPI_Reg_Read(RF_CH);
  Serial.print("RF_CH寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(RX_PW_P0, 32);
  tempValue = SPI_Reg_Read(RX_PW_P0);
  Serial.print("RX_PW_P0寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(RF_SETUP, 0x07);
  tempValue = SPI_Reg_Read(RF_SETUP);
  Serial.print("RF_SETUP寄存器为：");
  Serial.println(tempValue);
  SPI_Reg_Write(CONFIG, 0x0f);
  tempValue = SPI_Reg_Read(CONFIG);
  Serial.print("CONFIG寄存器为：");
  Serial.println(tempValue);
  digitalWrite(CE, 1);
}
