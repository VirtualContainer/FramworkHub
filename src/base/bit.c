#include"bit.h"

void Bitcopy(bit* Data_out, bit* Data_in, int Num) //二进制数组拷贝
{
    int i = 0;
    for(i = 0; i < Num; i++)
        Data_out[i] = Data_in[i];
}

void Change_bit(bit* Data, const int BitNum) //二进制的位置进行逆置；
{
    int i, j;
    static bit Temp[8] = {0};
    for(i = 0; i < BitNum / 8; i++)
    {
        Bitcopy(Temp, Data, BitNum / 8);
        for(j = 0; j < BitNum / 8; j++)
            Data[j] = Temp[BitNum / 8 - 1 - j];
        Data += BitNum / 8;
    }
}

void ByteToBit(bit* Data_out, char* Data_in,const int BitNum) //字节转位
{
    int i, j;
    for(i = 0; i < BitNum; i++)
        Data_out[i] = (Data_in[i / 8] >> (i % 8)) & 0x01;
}

void BitToHex(char* Data_out, bit* Data_in, int Num) //二进制转十六进制
{
    int i;
    for(i = 0; i < Num / 4; i++)
        Data_out[i] = 0;
    for(i = 0; i < Num / 4; i++)
    {
        Data_out[i] = Data_in[4 * i] + Data_in[4 * i + 1] * 2 + Data_in[4 * i + 2] * 4 + Data_in[4 * i + 3] * 8;
        if(Data_out[i] % 16 > 9)
        {
            Data_out[i] = Data_out[i] % 16 + '7';
        }
        else
            Data_out[i] = Data_out[i] % 16 + '0';
    }
}

void HexToBit(bit* Data_out, char* Data_in, const int Num) //十六进制转二进制
{
    int i;
    for(i = 0; i < Num; i++)
    {
        if(Data_in[i / 4] <= '9')
            Data_out[i] = ((Data_in[i / 4] - '0') >> (i % 4)) & 0x01;
        else
            Data_out[i] = ((Data_in[i / 4] - '7') >> (i % 4)) & 0x01;
    }
}

void BitToByte(char message_out[8], bit* Message_in,const int BitNum) //位转换成字节
{
    int i = 0;
    for(i = 0; i < (BitNum / 8); i++)
        message_out[i] = 0;
    for(i = 0; i < BitNum; i++)
        message_out[i / 8] |= Message_in[i] << (i % 8);
}

void Loop_bit(bit* Data_out, int movstep, const int len)
{
    static bit Temp[256] = {0};
    Bitcopy(Temp, Data_out, movstep);
    Bitcopy(Data_out, Data_out + movstep, len - movstep);
    Bitcopy(Data_out + len - movstep, Temp, movstep);
}

void Xor(bit* Message_out, bit* Message_in, const int BitNum)//执行异或
{
    int i;
    for(i = 0; i < BitNum; i++)
        Message_out[i] = Message_out[i] ^ Message_in[i];
}

void Add(bit* Message_out, bit* Message_in, const int BitNum)
{
    int i;
    for(i = 0; i < BitNum; i++)
        Message_out[i] = Message_out[i]&Message_in[i]; 
}