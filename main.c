#define 	FOSC		12000000L	//������ʱ�� 
#include	"STC15Fxxxx.H"
#define	Timer0_Reload	(65536 -(FOSC / 1000))//Timer 0 �ж�Ƶ��, 1000��/��
#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		  0x11


sbit LED1=P1^7;	                //����LED7��Ϊ���ӱ�־��ʱ�䵽����˸
sbit LED2=P1^6;	                //����LED6��Ϊ����ʱ��־��ʱ�䵽����˸

/*************	���ر�������	**************/
u8 	LED8[8];		    //��ʾ���� ʱ��
u8 	LED8_1[8];		  //��ʾ���� �¶�

u8  mode=1;         //mode=1 ʱ��    mode=2�¶�	 mode=3���� mode=4����ʱ
u8	display_index;	//��ʾλ����
u8  set_time=0;     //S7������ʱ������	0��ʱ����ʾ   1:ʱ��˸  2������˸  3������˸
u8  set_alarm=0;    //S6��������������    0��������ʾ   1:ʱ��˸  2������˸  3������˸

u8  set_count=0;    //S3����������ʱ����    0������ʱ��ʾ   1:ʱ��˸  2������˸  3������˸ 4:��ʼ����ʱ

u16 one_second;
u16 one_second_1;
bit	B_1ms;		    	//1ms��־
bit shanshuo_flag;  //ʱ��������˸��־
bit shanshuo_flag_1;//����������˸��־

bit shanshuo_flag_2;//����ʱ������˸��־

bit alarm_flag;     //���ӱ�־
bit count_flag;     //����ʱ��־λ

u8	hour,minute,second;

u8  alarm_hour,alarm_minute,alarm_second;

u8  count_hour,count_minute,count_second;

u16	msecond;
u16 msecond_1;
u16 ledshanshuo_count;
u16 ledshanshuo_count_2;  //����ʱ��˸
u16 led_count;
u16 led_count2;         //����ʱ��
u8	KeyCode;	      //���û�ʹ�õļ���, 1~16��Ч
u8	cnt10ms;
u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;	//���м��̱���
u8	cnt50ms;
/*************	���غ�������	**************/
u16	get_temperature(u16 adc);
u16	Get_ADC10bitResult(u8 channel);	          //channel = 0~7
void IO_KeyScan(void);                        //50ms call	 �������ɨ��
/*************	IO�ڶ���	**************/
sbit	P_HC595_SER   = P4^0;                 	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P5^4;                  	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P4^3;	                  //pin 11	SRCLK	Shift data clock


/*************	����ܶ�λ��	**************/
u8 code t_display[18]={						//����
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//  Ϩ��	 - 
   0x00,0x40};

u8 code T_COM[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//λ��

/********************** ��ʾʱ�Ӻ��� ************************/
void	DisplayRTC(void)
{
   if(mode==1)  //ʱ��
   {
		if((set_time==1)&&(shanshuo_flag==1))
		{
				LED8[0] = DIS_BLACK;
				LED8[1] = DIS_BLACK;
	    }
	    else
	    {
	   			LED8[0] = hour / 10;
			    LED8[1] = hour % 10;
	    }
		LED8[2] = DIS_;
		if((set_time==2)&&(shanshuo_flag==1))
		{
				LED8[3] = DIS_BLACK;
				LED8[4] = DIS_BLACK;
	    }
		else
		{
				LED8[3] = minute / 10;
				LED8[4] = minute % 10;
		}
		LED8[5] = DIS_;
		if((set_time==3)&&(shanshuo_flag==1))
		{
				LED8[6] = DIS_BLACK;
				LED8[7] = DIS_BLACK;	
		}
		else
		{
				LED8[6] = second / 10;
				LED8[7] = second % 10;	
		}
   }
   if(mode==2)  //�¶�  
   {
   	LED8[0]=DIS_BLACK;
	  LED8[1]=DIS_BLACK;
	  LED8[2]=DIS_BLACK;
	  LED8[3]=DIS_BLACK;
	  LED8[4]=DIS_BLACK;
	  LED8[5]=LED8_1[5];
	  LED8[6]=LED8_1[6];
	  LED8[7]=LED8_1[7];
   }
   
   if(mode==3)  //����
   {
		if((set_alarm==1)&&(shanshuo_flag_1==1))
		{
				LED8[0] = DIS_BLACK;
				LED8[1] = DIS_BLACK;
	    }
	    else
	    {
	   			LED8[0] = alarm_hour / 10;
			    LED8[1] = alarm_hour % 10;
	    }
		LED8[2] = DIS_;
		if((set_alarm==2)&&(shanshuo_flag_1==1))
		{
				LED8[3] = DIS_BLACK;
				LED8[4] = DIS_BLACK;
	  }
		else
		{
				LED8[3] = alarm_minute / 10;
				LED8[4] = alarm_minute % 10;
		}
		LED8[5] = DIS_;
		if((set_alarm==3)&&(shanshuo_flag_1==1))
		{
				LED8[6] = DIS_BLACK;
				LED8[7] = DIS_BLACK;	
		}
		else
		{
				LED8[6] = alarm_second / 10;
				LED8[7] = alarm_second % 10;	
		}
   }


   if(mode==4)  //����ʱ
   {
		if((set_count==1)&&(shanshuo_flag_2==1))
		{
				LED8[0] = DIS_BLACK;
				LED8[1] = DIS_BLACK;
	    }
	    else
	    {
	   			LED8[0] = count_hour / 10;
			    LED8[1] = count_hour % 10;
	    }
		LED8[2] = DIS_;
		if((set_count==2)&&(shanshuo_flag_2==1))
		{
				LED8[3] = DIS_BLACK;
				LED8[4] = DIS_BLACK;
	  }
		else
		{
				LED8[3] = count_minute / 10;
				LED8[4] = count_minute % 10;
		}
		LED8[5] = DIS_;
		if((set_count==3)&&(shanshuo_flag_2==1))
		{
				LED8[6] = DIS_BLACK;
				LED8[7] = DIS_BLACK;	
		}
		else
		{
				LED8[6] = count_second / 10;
				LED8[7] = count_second % 10;	
		}
   }
	 

   
   
   
   
}
//-------------ʱ����ʾ�Ľ���--------/
void RTC(void)
{
	if(++second >= 60)
	{
		second = 0;
		if(++minute >= 60)
		{
			minute = 0;
			if(++hour >= 24)	hour = 0;
		}
	}
}
//********����ʱ����**********
void RTB(void)
{ 
    if((count_hour==0)&&(count_minute==0)&&(count_second==0))
	{
		count_flag=1;
		 
	}
		else
    {
		  if(count_second-- <= 0)
	   {
	   count_second = 59;
		 
		 
		 if(count_minute-- <= 0)
		 {  
		   count_hour--;
		   count_minute = 59;
			 
			 }
			  if(count_hour <=0) 
				       hour=0;
				
				}
				count_flag=0;
				}			
}  

void main(void)
{
	u8	i;
	u8  k;
	u16	j;
	P0M1 = 0;	P0M0 = 0;	////�ϵ��ʼ������IO��Ϊ��ͨIO
	P1M1 = 0;	P1M0 = 0;	
	P2M1 = 0;	P2M0 = 0;	
	P3M1 = 0;	P3M0 = 0;	
	P4M1 = 0;	P4M0 = 0;
	P5M1 = 0;	P5M0 = 0;	
	P6M1 = 0;	P6M0 = 0;	
	P7M1 = 0;	P7M0 = 0;	
	AUXR = 0x80;	//Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;	        //Timer0 interrupt enable
	TR0 = 1;	        //Tiner0 run
	EA = 1;		        //�����ж�
	
  display_index = 0;
	P1ASF = 0x0C;		  //P1.2 P1.3��ADC
	ADC_CONTR = 0xE0;	//90T, ADC power on

	hour   = 23;	    //��ʼ��ʱ��ֵ
	minute = 59;
	second = 49;
  
  
	alarm_hour=0;
	alarm_minute=0;
	alarm_second=0;
  
  count_hour=0;
  count_minute=0;
  count_second=1;
  
	RTC();
	
	DisplayRTC();
	
	
  KeyCode = 0;	    //���û�ʹ�õļ���, 1~16��Ч
	cnt10ms = 0;
	IO_KeyState = 0;
	IO_KeyState1 = 0;
	IO_KeyHoldCnt = 0;
	cnt50ms = 0;
	k = 0;
	LED1=1;
	LED2=1;
	
  
  
  while(1)
	{
		if(B_1ms)	     //1ms��
		{
			B_1ms = 0;		
			if(mode==2)//�¶�
			{
				if(++msecond_1>= 300)	        //300ms��  �¶�
				{
					msecond_1 = 0;
					j = Get_ADC10bitResult(2);	//����0~7,��ѯ��ʽ��һ��ADC, ����ֵ���ǽ��, == 1024 Ϊ����					
					if(j < 1024)
					{
						LED8_1[0] = DIS_BLACK;		//��ʾADCֵ
						LED8_1[1] = DIS_BLACK;
						LED8_1[2] = DIS_BLACK;
						LED8_1[3] = DIS_BLACK;
						if(LED8_1[0] == 0)	LED8_1[0] = DIS_BLACK;
					}
					else	//����
					{
						for(i=0; i<4; i++)	LED8_1[i] = DIS_;
					}					
					j = Get_ADC10bitResult(3);	//����0~7,��ѯ��ʽ��һ��ADC, ����ֵ���ǽ��, == 1024 Ϊ����
					j += Get_ADC10bitResult(3);
					j += Get_ADC10bitResult(3);
					j += Get_ADC10bitResult(3);
					if(j < 1024*4)
					{
						j =	get_temperature(j);	 //�����¶�ֵ
	
						if(j >= 400)	F0 = 0,	j -= 400;	//�¶� >= 0��
						else			F0 = 1,	j  = 400 - j;	//�¶� <  0��
						LED8_1[4] = DIS_BLACK;		      //��ʾ�¶�ֵ
					  LED8_1[5] = (j % 1000) / 100;		//��ʾ�¶�ֵ
						LED8_1[6] = (j % 100) / 10;
	          LED8_1[7] = 12;			            //�ַ�"C"
					}
					else	//����
					{
						for(i=0; i<8; i++)	LED8_1[i] = DIS_;
					}
				}
			}
		}
	}
} 
/**********************************************/
u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};
void IO_KeyDelay(void)
{
	u8 i;
	i = 60;
	while(--i)	;
}
void IO_KeyScan(void)	    //50ms call
{
	u8	j;
	j = IO_KeyState1;	      //������һ��״̬
	P0 = 0xf0;	            //X�ͣ���Y
	IO_KeyDelay();
	IO_KeyState1 = P0 & 0xf0;
	P0 = 0x0f;	            //Y�ͣ���X
	IO_KeyDelay();
	IO_KeyState1 |= (P0 & 0x0f);
	IO_KeyState1 ^= 0xff;	  //ȡ��	
	if(j == IO_KeyState1)	  //�������ζ����
	{
		j = IO_KeyState;
		IO_KeyState = IO_KeyState1;
		if(IO_KeyState != 0)	//�м�����
		{
			F0 = 0;
			if(j == 0)	F0 = 1;	//��һ�ΰ���
			else if(j == IO_KeyState)
			{
				if(++IO_KeyHoldCnt >= 20)	//1����ؼ�
				{
					IO_KeyHoldCnt = 18;
					F0 = 1;
				}
			}
			if(F0)
			{
				j = T_KeyTable[IO_KeyState >> 4];
				if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
					KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;	//������룬17~32
			}
		}
		else	IO_KeyHoldCnt = 0;
	}
	P0 = 0xff;
}
//========================================================================
// ����: u16	Get_ADC10bitResult(u8 channel)
// ����: ��ѯ����һ��ADC���.
// ����: channel: ѡ��Ҫת����ADC.
// ����: 10λADC���.
// �汾: V1.0, 2012-10-22
//========================================================================
u16	Get_ADC10bitResult(u8 channel)	//channel = 0~7
{
	ADC_RES = 0;
	ADC_RESL = 0;
	ADC_CONTR = (ADC_CONTR & 0xe0) | 0x08 | channel; 	//start the ADC
	NOP(4);
	while((ADC_CONTR & 0x10) == 0)	;	//wait for ADC finish
	ADC_CONTR &= ~0x10;		            //���ADC������־
	return	(((u16)ADC_RES << 2) | (ADC_RESL & 3));
}
//	MF52E 10K at 25, B = 3950, ADC = 12 bits
u16 code temp_table[]={
		140,	//;-40	0
		149,	//;-39	1
		159,	//;-38	2
		168,	//;-37	3
		178,	//;-36	4
		188,	//;-35	5
		199,	//;-34	6
		210,	//;-33	7
		222,	//;-32	8
		233,	//;-31	9
		246,	//;-30	10
		259,	//;-29	11
		272,	//;-28	12
		286,	//;-27	13
		301,	//;-26	14
		317,	//;-25	15
		333,	//;-24	16
		349,	//;-23	17
		367,	//;-22	18
		385,	//;-21	19
		403,	//;-20	20
		423,	//;-19	21
		443,	//;-18	22
		464,	//;-17	23
		486,	//;-16	24
		509,	//;-15	25
		533,	//;-14	26
		558,	//;-13	27
		583,	//;-12	28
		610,	//;-11	29
		638,	//;-10	30
		667,	//;-9 	31
		696,	//;-8 	32
		727,	//;-7 	33
		758,	//;-6 	34
		791,	//;-5 	35
		824,	//;-4 	36
		858,	//;-3 	37
		893,	//;-2 	38
		929,	//;-1 	39
		965,	//;0  	40
		1003,	//;1  	41
		1041,	//;2	  42
		1080,	//;3	  43
		1119,	//;4	  44 
		1160,	//;5	  45
		1201,	//;6	  46
		1243,	//;7	  47
		1285,	//;8	  48
		1328,	//;9	  49 
		1371,	//;10	  50 
		1414,	//;11	  51
		1459,	//;12	  52
		1503,	//;13	  53
		1548,	//;14	  54
		1593,	//;15	  55 
		1638,	//;16	  56
		1684,	//;17	  57
		1730,	//;18	  58
		1775,	//;19	  59
		1821,	//;20	  60
		1867,	//;21	  61
		1912,	//;22	  62
		1958,	//;23	  63
		2003,	//;24	  64
		2048,	//;25	  65
		2093,	//;26	  66
		2137,	//;27	  67
		2182,	//;28	  68
		2225,	//;29	  69
		2269,	//;30	  70
		2312,	//;31	  71
		2354,	//;32	  72
		2397,	//;33	  73
		2438,	//;34 	74
		2479,	//;35	  75
		2519,	//;36 	76
		2559,	//;37	  77
		2598,	//;38  	78
		2637,	//;39	  79
		2675,	//;40 	80
		2712,	//;41 	81
		2748,	//;42 	82
		2784,	//;43 	83
		2819,	//;44 	84
		2853,	//;45 	85
		2887,	//;46 	86
		2920,	//;47 	87
		2952,	//;48 	88
		2984,	//;49 	89
		3014,	//;50 	90
		3044,	//;51 	91
		3073,	//;52 	92
		3102,	//;53 	93
		3130,	//;54 	94
		3157,	//;55 	95
		3183,	//;56 	96
		3209,	//;57 	97
		3234,	//;58 	98
		3259,	//;59 	99
		3283,	//;60 	100
		3306,	//;61 	101
		3328,	//;62 	102
		3351,	//;63 	103
		3372,	//;64 	104
		3393,	//;65 	105
		3413,	//;66 	106
		3432,	//;67 	107
		3452,	//;68 	108
		3470,	//;69 	109
		3488,	//;70 	110
		3506,	//;71 	111
		3523,	//;72 	112
		3539,	//;73 	113
		3555,	//;74 	114
		3571,	//;75 	115
		3586,	//;76 	116
		3601,	//;77 	117
		3615,	//;78 	118
		3628,	//;79 	119
		3642,	//;80 	120
		3655,	//;81 	121
		3667,	//;82 	122
		3679,	//;83 	123
		3691,	//;84 	124
		3702,	//;85 	125
		3714,	//;86 	126
		3724,	//;87 	127
		3735,	//;88 	128
		3745,	//;89 	129
		3754,	//;90 	130
		3764,	//;91 	131
		3773,	//;92 	132
		3782,	//;93 	133
		3791,	//;94 	134
		3799,	//;95 	135
		3807,	//;96 	136
		3815,	//;97 	137
		3822,	//;98 	138
		3830,	//;99 	139
		3837,	//;100 	140
		3844,	//;101  141
		3850,	//;102 	142
		3857,	//;103	143
		3863,	//;104	144
		3869,	//;105	145
		3875,	//;106	146
		3881,	//;107	147
		3887,	//;108	148
		3892,	//;109	149
		3897,	//;110	150
		3902,	//;111	151
		3907,	//;112	152
		3912,	//;113	153
		3917,	//;114	154
		3921,	//;115	155
		3926,	//;116	156
		3930,	//;117	157
		3934,	//;118	158
		3938,	//;119	159
		3942	//;120	160
};
/********************  �����¶� ***********************************************/
// ������: 0��Ӧ-40.0��, 400��Ӧ0��, 625��Ӧ25.0��, ���1600��Ӧ120.0��. 
// Ϊ��ͨ��, ADC����Ϊ12bit��ADCֵ.
// ��·������㷨���: Coody
/**********************************************/
#define		D_SCALE		10		//����Ŵ���, �Ŵ�10�����Ǳ���һλС��
u16	get_temperature(u16 adc)
{
	u16	code *p;
	u16	i;
	u8	j,k,min,max;
	adc = 4096 - adc; 	//Rt�ӵ�
	p = temp_table;
	if(adc < p[0])		return (0xfffe);
	if(adc > p[160])	return (0xffff);
	min = 0;		        //-40��
	max = 160;	      	//120��
	for(j=0; j<5; j++)	//�Էֲ��
	{
		k = min / 2 + max / 2;
		if(adc <= p[k])	max = k;
		else			min = k;
	}
		 if(adc == p[min])	  i = min * D_SCALE;
	else if(adc == p[max])	i = max * D_SCALE;
	else	// min < temp < max
	{
		while(min <= max)
		{
			min++;
			if(adc == p[min])	{i = min * D_SCALE;	break;}
			else if(adc < p[min])
			{
				min--;
				i = p[min];	//min
				j = (adc - i) * D_SCALE / (p[min+1] - i);
				i = min;
				i *= D_SCALE;
				i += j;
				break;
			}
		}
	}
	return i;
}



//********��������**********

void key_control()
{
	if(KeyCode > 0)	    //�м�����
	{	
		if(alarm_flag==1)
		{
			led_count=0;
			LED1=1;
			ledshanshuo_count=0;
			alarm_flag=0;		
		} 
		if(count_flag==1)
		{
			led_count2=0;
			LED2=1;
			ledshanshuo_count_2=0;
			count_flag=0;		
		} 
		
		
		if(KeyCode == 20)	//S3 ����ʱ����
  {  
      set_alarm=0;
      mode=4;
      set_count++;
			if(set_count==5)
			{
				set_count=0;
				
				
				
			}	
    }
		
		if(KeyCode == 24)	//S7ʱ������  
		{
			set_alarm=0;
			mode=1;         //ʱ��
			set_time++;
			if(set_time==4)
			{
				set_time=0;
			}			
		}
		if(KeyCode == 23)	//S6��������  
		{
			  set_time=0;
			  mode=3;       //����
			  set_alarm++;
			  if(set_alarm==4)
				{
			  	 set_alarm=0;
				 mode=1;      //ʱ��
			  }
		}
    
    
		if(KeyCode == 22)	//S5 ��        
		{
		
		if (mode==4)	    //����ʱ
			{
				if(set_count==1)
				{
					if(++count_hour >= 24)	count_hour = 0;
				}
				if(set_count==2)
				{
					if(++count_minute >= 60) count_minute = 0;
				}
				if(set_count==3)
				{
					if(++count_second >= 60) count_second = 0;
				}														
		   }
			if(mode==1)//ʱ��
			{
				if(set_time==1)
				{
					if(++hour >= 24)	hour = 0;
				}
				if(set_time==2)
				{
					if(++minute >= 60)	minute = 0;
				}
				if(set_time==3)
				{
					if(++second >= 60)	second = 0;
				}														
		   }
			if(mode==3)	    //����
			{
				if(set_alarm==1)
				{
					if(++alarm_hour >= 24)	alarm_hour = 0;
				}
				if(set_alarm==2)
				{
					if(++alarm_minute >= 60)alarm_minute = 0;
				}
				if(set_alarm==3)
				{
					if(++alarm_second >= 60)alarm_second = 0;
				}														
		   }
       
       
		}
		if(KeyCode == 21)	//S4 ��   	��ʱ��״̬�°���S4������ʾ�¶ȣ��ɿ���ʾʱ��
		{				
			if(set_time==0)
			{
			   if(mode==1)  //������Ŀ��˼mode==1&&set_time==0 ��֤���ڡ�ʱ����ʾ������
			   {
			   	   mode=2;  //�¶���ʾ����
			   }
			}
			if(mode==1)
			{
				if(set_time==1)
				{
					if(--hour>=24)	hour = 23;
				}
				if(set_time==2)	
				{
					if(--minute>=60)	minute = 59;
				}
				if(set_time==3)	
				{
					if(--second >=60)	second = 59;
				}												
			}
			if(mode==3)
			{
				if(set_alarm==1)
				{
					if(--alarm_hour >=24)	alarm_hour = 23;
				}
				if(set_alarm==2)	
				{
					if(--alarm_minute >=60)alarm_minute = 59;
				}
				if(set_alarm==3)	
				{
					if(--alarm_second >=60)alarm_second = 59;
				}												
			}
      
      if(mode==4)
			{
				if(set_count==1)
				{
					if(--count_hour >=24)	count_hour = 23;
				}
				if(set_count==2)	
				{
					if(--count_minute >=60)count_minute = 59;
				}
				if(set_count==3)	
				{
					if(--count_second >=60)count_second = 59;
				}												
			}
      
		}
			KeyCode = 0;	
		DisplayRTC();
	}	
	else
	{
		if((mode==2)&&(set_time==0))  //�������ּ��
		{
			mode=1;	                  	//�ص�ʱ����ʾ
		}
	}
  
  
 


}    
      
       
/**************** ��HC595����һ���ֽں��� ******************/
void Send_595(u8 dat)
{		
	u8	i;
	for(i=0; i<8; i++)
	{
		dat <<= 1;
		P_HC595_SER   = CY;
		P_HC595_SRCLK = 1;
		P_HC595_SRCLK = 0;
	}
}
/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{	
	Send_595(~T_COM[display_index]);			  	    //���λ��
  Send_595(t_display[LED8[display_index]]);	    //�������
	P_HC595_RCLK = 1;
	P_HC595_RCLK = 0;							                //�����������
	if(++display_index >= 8)	display_index = 0;	//8λ������0
}
/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt TIMER0_VECTOR
{
	DisplayScan();	                              //1msɨ����ʾһλ
	B_1ms = 1;	                                	//1ms��־
	if(++msecond >= 1000)	                        //1�뵽
	{
		msecond = 0;
		RTC();
		if(set_count==4)
		 {
		  RTB();
		  }
		if((hour==alarm_hour)&&(minute==alarm_minute)&&(second==alarm_second))
		{
			alarm_flag=1;
		}
		
		
		DisplayRTC();
	}
	if(++cnt10ms >= 10)		 //50msɨ��һ�����м���
	{
		cnt10ms = 0;
		IO_KeyScan();
		key_control();
	}
	if(mode==1)            //ʱ��������˸
	{
		if(set_time!=0)
		{
			if(++one_second>=1000)
			{
				 one_second=0;
				 shanshuo_flag=~shanshuo_flag;
			}
		}
	}
	if(mode==3)            //����������˸
	{
	   	if(set_alarm!=0)
		{
			if(++one_second_1>=1000)
			{
				 one_second_1=0;
				 shanshuo_flag_1=~shanshuo_flag_1;
			}
		}
	}
  	if(alarm_flag==1)
	{
		if(++led_count<=5000)
		{
			if(++ledshanshuo_count<=400)
			{
				if(ledshanshuo_count<=200)
				{
					LED1=0;
				}
				else
				{
					LED1=1;
				}
			}
			else
			{
				ledshanshuo_count=0;
			}
		}
		else
		{
			led_count=0;
			LED1=1;
			ledshanshuo_count=0;
			alarm_flag=0;
		}
	}

if(mode==4)            //����������˸
	{
	   	if(set_count!=0)
		{
			if(++one_second_1>=1000)
			{
				 one_second_1=0;
				 shanshuo_flag_2=~shanshuo_flag_2;
			}
		}
	}
	
	
	
  	if(count_flag==1)
	{
		if(++led_count2<=5000)
		{
			if(++ledshanshuo_count_2<=400)
			{
				if(ledshanshuo_count_2<=200)
				{
					LED2=0;
				}
				else
				{
					LED2=1;
				}
			}
			else
			{
				ledshanshuo_count_2=0;
			}
		}
if(count_flag==0)
		{
			led_count2=0;
			LED2=0;
			ledshanshuo_count_2=0;
			count_flag=0;
		}
	}
		
	
	
  	
}


