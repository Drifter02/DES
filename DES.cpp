#include <stdlib.h>
#include <stdio.h>
#include "DES_tables.h"

static bool SubKey[16][48]= {0}; //����Կ����

void BitsCopy(bool *BitIn,bool *BitOut,int Len) {  //BitOut ����IP�û������Ϣ 
//���鸴��
	int i=0;
	for(i=0; i<Len; i++) {
		BitOut[i]=BitIn[i];
	}
}
void TablePermute(bool *BitIn,bool *BitOut,const char *Table,int Num) {  //IP�û� 
//ԭ�������ݰ���Ӧ�ı��ϵ�λ�ý��з���
	int i=0;
	static bool temp[256]= {0};
	for(i=0; i<Num; i++) {
		temp[i]=BitIn[Table[i]-1];
	}
	BitsCopy(temp,BitOut,Num);
}

void ByteToBit(char *ByteIn,bool *BitOut,int Num) {
//���ֽ�תΪByte
	int i=0;
	for(i=0; i<Num; i++) {
		BitOut[i]=(ByteIn[i/8]>>(i%8))&0x01;  //������0x01����������������õ���0��1 
	}
}
void LoopMove(bool *BitIn,int Len,int Num) {    //ѭ�����Ʋ��� 
//��λ������ע���轫��ߵ���1��2λ�Ƶ����ұ�
	static bool temp[256]= {0};
	BitsCopy(BitIn,temp,Num);
	BitsCopy(BitIn+Num,BitIn,Len-Num);
	BitsCopy(temp,BitIn+Len-Num,Num);
}
void Getsubkey(char KeyIn[8]) {             //��������Կ 
//ͨ����Կ�������Կ
	int i=0;
	static bool KeyBit[64]= {0};              
	static bool *KiL=&KeyBit[0],*KiR=&KeyBit[28];
	ByteToBit(KeyIn,KeyBit,64);                    //����ԿתΪbits 
	TablePermute(KeyBit,KeyBit,Subkey_Table,56);   //����Կѹ��������ÿһ���ַ��ĵڰ�λ 
	for(i=0; i<16; i++) {                          //����Կ�ĸ�28λ�͵�28λ�ֱ����ѭ�����ƣ����ƴ�������Ӧ��Move_table�� 
		LoopMove(KiL,28,Move_Table[i]);
		LoopMove(KiR,28,Move_Table[i]);
		TablePermute(KeyBit,SubKey[i],Compress_Table,48); //���õ��ĵ�i������Կ�ŵ�subKey[i]�� 
	}
}

void Xor(bool *Bit1,bool *Bit2,int Num) {
// ��λ��򣬴洢����ڵ�һ����
	int i=0;
	for(i=0; i<Num; i++) {
		Bit1[i]=Bit1[i]^Bit2[i];
	}
}
void S_Change(bool BitIn[48],bool BitOut[32]) {
// S�б任����48λ�Ĵ�����ѹ����32λ 
	int i,X,Y;
	for(i=0,Y=0,X=0; i<8; i++,BitIn+=6,BitOut+=4) {
		Y=(BitIn[0]<<1)+BitIn[5];                            //1��6λ��������
		X=(BitIn[1]<<3)+(BitIn[2]<<2)+(BitIn[3]<<1)+BitIn[4];//2345��������
		ByteToBit(&S_Box[i][Y][X],BitOut,4);
	}
}
void DES_1turn(bool BitIn[32],bool BitKi[48]) {  //�Ұ����չ��������Կ������� 
	static bool MiR[48]= {0};         
	TablePermute(BitIn,MiR,Ex_Table,48);        //��չΪ48λ
	Xor(MiR,BitKi,48);                          //������ 
	S_Change(MiR,BitIn);                        //S�д���ѹ�� 
	TablePermute(BitIn,BitIn,P_Box,32);         //P�û������ߺ��Ұ�ߵĴ������������Ȼ�����ҽ�����һ�־������� 
}

void BitToHex(bool *BitIn,char *ByteOut,int Num) {
//BitתHex
	int i=0;
	for(i=0; i<Num/4; i++) {
		ByteOut[i]=0;
	}
	for(i=0; i<Num/4; i++) {             //���ö�������������õ���������ת��Ϊ��Ӧ��char�� 
		ByteOut[i] = BitIn[i*4]+(BitIn[i*4+1]<<1)
		             +(BitIn[i*4+2]<<2)+(BitIn[i*4+3]<<3);
		if((ByteOut[i])>9) {
			ByteOut[i]=ByteOut[i]+'7';  //��������ASCII�� �����ֺ���ĸ֮�����������ŵ�ԭ�� 
		} else {
			ByteOut[i]=ByteOut[i]+'0';
		}
	}
}
void BitToByte(bool *ByteIn,char *BitOut,int Num) {
//ÿ8������һλ���
	int i=0;
	for(i=0; i<(Num/8); i++) {
		BitOut[i]=0;
	}
	for(i=0; i<Num; i++) {
		BitOut[i/8]|=ByteIn[i]<<(i%8);
	}
}
void HexToBit(char *ByteIn,bool *BitOut,int Num) {
//HexתBit
	int i=0;
	for(i=0; i<Num; i++) {
		if((ByteIn[i/4])>'9') {
			BitOut[i]=((ByteIn[i/4]-'7')>>(i%4))&0x01;    //��������ԭ�� 
		} else {
			BitOut[i]=((ByteIn[i/4]-'0')>>(i%4))&0x01;     
		}
	}
}
void DES_Cry(char MesIn[8],char MesOut[8]) {
//ִ��DES���ܺ���
	int i=0;
	static bool MesBit[64]= {0};                  //��Ϣ 
	static bool Temp[32]= {0};                    //�м���� 
	static bool *MiL=&MesBit[0],*MiR=&MesBit[32]; //ǰ��32λ
	ByteToBit(MesIn,MesBit,64);                   //charתbit��MesBit�� 
	TablePermute(MesBit,MesBit,IP_Table,64);      //IP�û�������Ϣ���д�λ 
	for(i=0; i<16; i++) {                         //16�ֵ���
		BitsCopy(MiR,Temp,32);                    //�Ұ�߸��Ƶ���ʱ����temp 
		DES_1turn(MiR,SubKey[i]);                 //�Ұ����չ������Կ�������Ȼ��ѹ�� 
		Xor(MiR,MiL,32);                          //�������ŵ��ұ� 
		BitsCopy(Temp,MiL,32);                    //һ��ʼ���ұ����ݷŵ���� 
	}
	TablePermute(MesBit,MesBit,IPre_Table,64);    //IP���û� 
	BitToHex(MesBit,MesOut,64);                   //��16����������� 
}

void DES_Dec(char MesIn[8],char MesOut[8]) {
//DES���ܣ����ܵ������ doublesand 
	int i=0;
	static bool MesBit[64]= {0};
	static bool Temp[32]= {0};
	static bool *MiL=&MesBit[0],*MiR=&MesBit[32];
	HexToBit(MesIn,MesBit,64);                     //16��������ת������ 
	TablePermute(MesBit,MesBit,IP_Table,64);       //IP�û� 
	for(i=15; i>=0; i--) {                         //��ѭ�� 
		BitsCopy(MiL,Temp,32);                     //R(i-1) = Li, L15�����ĵ�ǰ��Σ��������� 
		DES_1turn(MiL,SubKey[i]);                  //Ri = L(i-1)^f(R(i-1), K(i-1))   K(i-1)������Կ��R15��֪������ a = b ^ c �� b = a^c,������L(i-1) 
		Xor(MiL,MiR,32);                           //�������õ��ұߵ�ԭʼ��Ϣ�Ż���� 
		BitsCopy(Temp,MiR,32);                     //�м�����ŵ��ұ� 
	}
	TablePermute(MesBit,MesBit,IPre_Table,64);     //IP���û� 
	BitToByte(MesBit,MesOut,64);                   //������תchar 
}

int main() {
	int i=0;
	char MesHex[16]= {0};     //�������
	char MyKey[8]= {0};       //��ʼ��Կ
	char YourKey[8]= {0};     //������Կ
	char MyMessage[8]= {0};   //����
	printf("����������(������ 64 bit):\n");
	gets(MyMessage);//����
	printf("������Կ(64 bit):\n");
	gets(MyKey);//��Կ
	Getsubkey(MyKey);            //��������Կ����������ִ�� 
 	DES_Cry(MyMessage,MesHex);   //���ܹ��� 
	printf("��������:\n");
	for(i=0; i<16; i++) {
		printf("%c ",MesHex[i]);
	}
	printf("\n");
	printf("������Կ����:\n");
	gets(YourKey);//get��Կ
	Getsubkey(YourKey);   //��������Կ 
	DES_Dec(MesHex,MyMessage);  //���ܹ��� 
	printf("������� !!:\n");
	for(i=0; i<8; i++) {
		printf("%c ",MyMessage[i]);
	}
	printf("\n");
	system("pause");
}
