#include "hpd.h"
#include "uart.h"
extern u8 Spec_contrl;

extern u8 EQConfig;
extern uint8_t i2c_addr_pool[8];
extern u8 temp_eq_data,temp_equp_data;
extern u8 MemData[];
extern u8 NewPort;
extern u8  enableGain,ScanPort,ScanFlag,MemFlag,BuzzFlag;
extern u16 Interval_MemFlag,Interval_PluginFlag;


static u16 input_flag = 0X00; //重复拔插标志
static u16 redet_temp = 0;    //拔插后，重走检测流程标志
u8  lock_lastport;
u8  TxPlugoutFlag  = 1;     //输出口未检测到插入标志，1->未检测到插入标志
u8  Global_portnum = 0xFF;  //!!!仅适用同一口上下板只有一个rxin的情况。16口全局编号，下板+上板0~7 ->>> 0~15
u8  Global_portnum1 = 0xff;
u16 Global_num     = 0x00;  //HPD单独控制变量：0-7->下板rx,,8-15->上板rx。用于H1模式hpd控制
u16 h_mode_flag = 0X0000;		//for h1 mode control

u8 port_count0,port_count1; //for 2port rxin in the same time
u8 twoTxPlugoutFlag0 = 1,twoTxPlugoutFlag1 = 1; //输出口未检测到插入标志，1->未检测到插入标志

u8 Lastport_tem0=0xff,Lastport_tem1=0xff;				//tworx_Tx_Hpd_Detect()上一个select端口变量
u8 lock=0;																			//1:2rx in,使Lastport_tem和Global_portnum始终相等，不进入Tx_Hpd_Detect()
u8 Tx_Hpd_reDetect = 0xFF;//Tx_Hpd_reDetect状态：0xff->切换至当前口，但未插入\
                                             \   1 ->RX in,等待TX插入            \
                                             \   0 ->TX已插入

/*******************************************************************************
  * @FunctionName: Tx_Hpd_Detect
  * @Author:       ftansiX
  * @DateTime:     2022年7月1日T13:31:28+0800
  * @Purpose:      
  * @param:        Glo_port : Global_portnum全局序号  
  * @note:         port改变时，先检测RX in,然后检测TX in
*******************************************************************************/
u8 Lastport_tem=0xff;							//Tx_Hpd_Detect()上一个select端口变量	
u16 Port_selected_state=0;       //for 2rx-in only,端口激活且检测到tx in之后，相应位置1。
extern u8 Lastport_tem0,Lastport_tem1;//extern u8 port_count0,port_count1;
u8 last=0xff;
extern u8 lock;
void Tx_Hpd_Detect(u8 Glo_port)
{
  //static u16 h_mode_flag = 0X0000;
	u8 tx_hpd[2]={0};
  u8 rx_upbod_chip1=0,rx_upbod_chip2=0,rx_upbod_chip3=0;
  u8 rx_downbod_chip1=0,rx_downbod_chip2=0,rx_downbod_chip3=0;
  if(Lastport_tem!= Glo_port)
	{ /* select port后再拉低hpd */
     Switch_Port_GPIO(0,NewPort);
     delay1ms(5);
     Switch_Port_GPIO(1,NewPort);
   
    /*END*/
    temp_eq_data = 0Xf0;
    temp_equp_data = 0xf0;
    
    Tx_Hpd_reDetect=1;
		if(HpdMode==1)	    //H1 mode->切换时闪屏模式
		{
			printf("->H1 Mode \r\n");
      Set_OtherHpd_Low(NewPort);
      h_mode_flag = 0;
		}
		else if(HpdMode==0) //H0 mode->切换不闪屏模式
		{
			printf("->H0 Mode\r\n");
		}	
    lock=0;
	}
  
	Lastport_tem=Glo_port;
  {Lastport_tem0=0xff; Lastport_tem1=0xff;}
  
#if 1
  rx_upbod_chip1 = MS9601A_Up_Read(addr_up_0,0x51);
  rx_upbod_chip2 = MS9601A_Up_Read(addr_up_1,0x51);
  rx_upbod_chip3 = MS9601A_Up_Read(addr_up_2,0x51);

  rx_downbod_chip1 = MS9601A_Down_Read(addr_down_0,0x51);
  rx_downbod_chip2 = MS9601A_Down_Read(addr_down_1,0x51);
  rx_downbod_chip3 = MS9601A_Down_Read(addr_down_2,0x51);

	if(Tx_Hpd_reDetect==1)TxPlugoutFlag=1;
	if(TxPlugoutFlag==1)
	{
    //close rx-tx power for tx HPD detect
    if(Glo_port>13)//up-port6&7
    {
  		MS9601A_Up_Write(i2c_addr_pool[6],0x20, 0x03);
  		MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else if(Glo_port>10)//up-port3&4&5
    {
      MS9601A_Up_Write(i2c_addr_pool[5],0x20, 0x03);
      MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else if(Glo_port>7)//up-port0&1&2
    {
      MS9601A_Up_Write(i2c_addr_pool[4],0x20, 0x03);
      MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else if(Glo_port>5)//down-port6&7
    {
  		MS9601A_Down_Write(i2c_addr_pool[2],0x20, 0x03);
  		MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    else if(Glo_port>2)//down-port3&4&5
    {
      MS9601A_Down_Write(i2c_addr_pool[1],0x20, 0x03);
      MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    else//down-port0&1&2
    {
      MS9601A_Down_Write(i2c_addr_pool[0],0x20, 0x03);
      MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    delay1ms(5);
      
		tx_hpd[1] = MS9601A_Up_Read(i2c_addr_pool[7],0x50);
		tx_hpd[0] = MS9601A_Down_Read(i2c_addr_pool[3],0x50);
    
		//printf("NewPort->%u,rx_downbod->0x%02x,scan--txIN--tx_hpd[0]=%02x \r\n",NewPort,rx_downbod_chip1,tx_hpd[0]);
    /*当前激活口，上下板检测（单独检测）RX in----------------------*/
    if((rx_downbod_chip1&0x01)&&(Glo_port==0))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip1&0x02)&&(Glo_port==1))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip1&0x04)&&(Glo_port==2))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip2&0x01)&&(Glo_port==3))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip2&0x02)&&(Glo_port==4))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip2&0x04)&&(Glo_port==5))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip3&0x01)&&(Glo_port==6))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip3&0x02)&&(Glo_port==7))
      if(tx_hpd[0]>>4&0X01)goto re_setport;

    if((rx_upbod_chip1&0x01)&&(Glo_port==8))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip1&0x02)&&(Glo_port==9))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip1&0x04)&&(Glo_port==10))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip2&0x01)&&(Glo_port==11))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip2&0x02)&&(Glo_port==12))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip2&0x04)&&(Glo_port==13))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip3&0x01)&&(Glo_port==14))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip3&0x02)&&(Glo_port==15))
      if(tx_hpd[1]>>4&0X01)goto re_setport;

    Port_selected_state&=~(1<<Glo_port);//0818
    
    return;
      
re_setport:
    printf("TX is insert \r\n");
    TxPlugoutFlag = 0;                    //检测到tx插入后置0 
    Tx_Hpd_reDetect = 0;
    //printf("h=%x\r\n",h_mode_flag);
    if(HpdMode==1)Set_ThisHpd_Low(Global_portnum);         
    else if(HpdMode==0&&(h_mode_flag>>Glo_port&0x01)==0)  
    {
      Set_ThisHpd_Low(Global_portnum);	  //H0模式下跳过这步，不拉低HPD。具体情况:当前口->其它口->当前口
      //printf("check1\r\n");
    }
    delay1ms(5);//20220801 hpd拉低时间？

		if(port_count0==NewPort)
		{
	    Switch_Port_GPIO(0,NewPort); 
	  	if(HpdMode==1){Force_Control_Rx0_Rx1_HPD(0,NewPort);}                   
	    else if(HpdMode==0&&(h_mode_flag>>Glo_port&0x01)==0)  
	    {
	    	Force_Control_Rx0_Rx1_HPD(0,NewPort);
	      h_mode_flag|=1<<Glo_port;         //H0模式下，端口切换后运行一次hdmi_rx_det()
	      //printf("check2\r\n");
	    }
			Port_selected_state&=~0x00ff;
			Port_selected_state|=1<<port_count0;//设置下板select状态
		}
		else if(port_count1==NewPort)
		{
	    Switch_Port_GPIO(1,NewPort);
	  	if(HpdMode==1){Force_Control_Rx0_Rx1_HPD(1,NewPort);}                    
	    else if(HpdMode==0&&(h_mode_flag>>Glo_port&0x01)==0)  
	    {
	    	Force_Control_Rx0_Rx1_HPD(1,NewPort);
	      h_mode_flag|=1<<Glo_port;      		//H0模式下，端口切换后运行一次hdmi_rx_det()
	      //printf("check3\r\n");
	    }
			Port_selected_state&=~0xff00;
			Port_selected_state|=1<<(port_count1+8);//设置上板select状态
		}
#if (port2flash==0)
		MemFlag=2;//当前口无RX in,不写flash
#endif
	}
#endif
}
extern u8 ds_v_re;
/*******************************************************************************
  * @FunctionName: Tx_Hpd_Detect
  * @Author:       ftansiX
  * @DateTime:     2022年7月1日T13:31:28+0800
  * @Purpose:      部分电脑(AMD)，热键修改增益时会雪花屏,切到其它口再切回后正常
  * @param:        Glo_port : Global_portnum全局序号  
  * @note:         端口+1-->EQchangeTx_Hpd_Detect()-->端口+1-1-->EQchangeTx_Hpd_Detect()-->TX in
*******************************************************************************/
void EQchangeTx_Hpd_Detect(u8 Glo_port)
{
  //static u16 h_mode_flag = 0X0000;
  static u8 Lastpo=0xff,TxPlu=1;
  u8 temp_NewPort;
	u8 tx_hpd[2]={0};
  u8 rx_upbod_chip1=0,rx_upbod_chip2=0,rx_upbod_chip3=0;
  u8 rx_downbod_chip1=0,rx_downbod_chip2=0,rx_downbod_chip3=0;
  if(Lastpo!= Glo_port)
	{ /* select port后再拉低hpd */
			if(Glo_port<8)
			{
				temp_NewPort =Glo_port;
				Switch_Port_GPIO(0,temp_NewPort);
				delay1ms(5);
			}
		  else if(Glo_port<16)
		  {
				temp_NewPort =Glo_port-8;
				Switch_Port_GPIO(1,temp_NewPort);
		  }
   
    /*END*/
//    temp_eq_data = 0Xf0;
//    temp_equp_data = 0xf0;

    ds_v_re=0;
    TxPlu=1;
		if(HpdMode==1)	    //H1 mode->切换时闪屏模式
		{
			printf("->H1 Mode \r\n");
      singleSet_OtherHpd_Low(Glo_port);
      h_mode_flag = 0;
		}
		else if(HpdMode==0) //H0 mode->切换不闪屏模式
		{
			printf("->H0 Mode\r\n");
		}	
    lock=0;
	}
  
	Lastpo=Glo_port;
//  {Lastport_tem0=0xff; Lastport_tem1=0xff;}
  
#if 1
  rx_upbod_chip1 = MS9601A_Up_Read(addr_up_0,0x51);
  rx_upbod_chip2 = MS9601A_Up_Read(addr_up_1,0x51);
  rx_upbod_chip3 = MS9601A_Up_Read(addr_up_2,0x51);

  rx_downbod_chip1 = MS9601A_Down_Read(addr_down_0,0x51);
  rx_downbod_chip2 = MS9601A_Down_Read(addr_down_1,0x51);
  rx_downbod_chip3 = MS9601A_Down_Read(addr_down_2,0x51);

	if(TxPlu==1)
	{
    //close rx-tx power for tx HPD detect
    if(Glo_port>13)//up-port6&7
    {
  		MS9601A_Up_Write(i2c_addr_pool[6],0x20, 0x03);
  		MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else if(Glo_port>10)//up-port3&4&5
    {
      MS9601A_Up_Write(i2c_addr_pool[5],0x20, 0x03);
      MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else if(Glo_port>7)//up-port0&1&2
    {
      MS9601A_Up_Write(i2c_addr_pool[4],0x20, 0x03);
      MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else if(Glo_port>5)//down-port6&7
    {
  		MS9601A_Down_Write(i2c_addr_pool[2],0x20, 0x03);
  		MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    else if(Glo_port>2)//down-port3&4&5
    {
      MS9601A_Down_Write(i2c_addr_pool[1],0x20, 0x03);
      MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    else//down-port0&1&2
    {
      MS9601A_Down_Write(i2c_addr_pool[0],0x20, 0x03);
      MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    delay1ms(5);
      
		tx_hpd[1] = MS9601A_Up_Read(i2c_addr_pool[7],0x50);
		tx_hpd[0] = MS9601A_Down_Read(i2c_addr_pool[3],0x50);
    
		//printf("NewPort->%u,rx_downbod->0x%02x,scan--txIN--tx_hpd[0]=%02x \r\n",NewPort,rx_downbod_chip1,tx_hpd[0]);
    /*当前激活口，上下板检测（单独检测）RX in----------------------*/
    if((rx_downbod_chip1&0x01)&&(Glo_port==0))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip1&0x02)&&(Glo_port==1))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip1&0x04)&&(Glo_port==2))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip2&0x01)&&(Glo_port==3))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip2&0x02)&&(Glo_port==4))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip2&0x04)&&(Glo_port==5))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip3&0x01)&&(Glo_port==6))
      if(tx_hpd[0]>>4&0X01)goto re_setport;
    if((rx_downbod_chip3&0x02)&&(Glo_port==7))
      if(tx_hpd[0]>>4&0X01)goto re_setport;

    if((rx_upbod_chip1&0x01)&&(Glo_port==8))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip1&0x02)&&(Glo_port==9))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip1&0x04)&&(Glo_port==10))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip2&0x01)&&(Glo_port==11))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip2&0x02)&&(Glo_port==12))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip2&0x04)&&(Glo_port==13))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip3&0x01)&&(Glo_port==14))
      if(tx_hpd[1]>>4&0X01)goto re_setport;
    if((rx_upbod_chip3&0x02)&&(Glo_port==15))
      if(tx_hpd[1]>>4&0X01)goto re_setport;

//    Port_selected_state&=~(1<<Glo_port);//0818
    
    return;
      
re_setport:
    printf("EQ: TX is insert \r\n");
    TxPlu = 0;                    //检测到tx插入后置0 
//    Tx_Hpd_reDetect = 0;
    //printf("h=%x\r\n",h_mode_flag);
//    if(HpdMode==1)Set_ThisHpd_Low(Glo_port);         
//    else if(HpdMode==0&&(h_mode_flag>>Glo_port&0x01)==0)  
    {
      Set_ThisHpd_Low(Glo_port);	  //H0模式下跳过这步，不拉低HPD。具体情况:当前口->其它口->当前口
      //printf("check1\r\n");
    }
    delay1ms(5);//20220801 hpd拉低时间？

		if((port_count0==temp_NewPort)&&(Glo_port<8))
		{
	    Switch_Port_GPIO(0,temp_NewPort); 
//	  	if(HpdMode==1){Force_Control_Rx0_Rx1_HPD(0,NewPort);}                   
//	    else if(HpdMode==0&&(h_mode_flag>>Glo_port&0x01)==0)  
	    {
	    	Force_Control_Rx0_Rx1_HPD(0,temp_NewPort);
//	      h_mode_flag|=1<<Glo_port;         //H0模式下，端口切换后运行一次hdmi_rx_det()
	      //printf("check2\r\n");
	    }
//			Port_selected_state&=~0x00ff;
//			Port_selected_state|=1<<port_count0;//设置下板select状态
		}
		else if((port_count1==temp_NewPort)&&(Glo_port<16))
		{
	    Switch_Port_GPIO(1,temp_NewPort);
//	  	if(HpdMode==1){Force_Control_Rx0_Rx1_HPD(1,NewPort);}                    
//	    else if(HpdMode==0&&(h_mode_flag>>Glo_port&0x01)==0)  
	    {
	    	Force_Control_Rx0_Rx1_HPD(1,temp_NewPort);
//	      h_mode_flag|=1<<Glo_port;      		//H0模式下，端口切换后运行一次hdmi_rx_det()
	      //printf("check3\r\n");
	    }
//			Port_selected_state&=~0xff00;
//			Port_selected_state|=1<<(port_count1+8);//设置上板select状态
		}
		EQConfig=0;
	}
#endif
}

void tworx_Tx_Hpd_Detect(u8 d_port,u8 up_port)
{ u8 flg=0;
	u8 tx_hpd[2]={0};
  u8 rx_upbod_chip1=0,rx_upbod_chip2=0,rx_upbod_chip3=0;
  u8 rx_downbod_chip1=0,rx_downbod_chip2=0,rx_downbod_chip3=0;
  if((Lastport_tem0!= d_port) && (Lastport_tem1!=up_port))//Spec_contrl add0920
	{ /* select port后再拉低hpd */
		
		
    if((Port_selected_state>>(d_port)&0x01)==0)Switch_Port_GPIO(0,NewPort); 
    delay1ms(5);
    if((Port_selected_state>>(up_port+8)&0x01)==0)Switch_Port_GPIO(1,NewPort);
    /*END*/
    temp_eq_data = 0Xf0;
    temp_equp_data = 0xf0;

		if((Port_selected_state>>(d_port)&0x01)==0)
      twoTxPlugoutFlag0=1;
		if((Port_selected_state>>(up_port+8)&0x01)==0)
      twoTxPlugoutFlag1=1;
    
		if(HpdMode==1)	    //H1 mode->切换时闪屏模式
		{ printf("->2H1 Mode \r\n");
      Set_OtherHpd_Low(NewPort);//不包含同上下板的同一口
      h_mode_flag = 0;
		}
		else if(HpdMode==0) //H0 mode->切换不闪屏模式
		{ printf("->2H0 Mode\r\n");
		}		
	}
  /* for 1rx in */
  Lastport_tem  =d_port;//or up_port
  Global_portnum=d_port;//or up_port
  lock_lastport =d_port;
  /* for 2rx in */
  Lastport_tem0 =d_port;
  Lastport_tem1 =up_port;

  //printf("d_port=%u,up_port=%u \r\n",d_port,up_port);
  //printf("scan--txIN--=%02x \r\n",MS9601A_Down_Read(i2c_addr_pool[3],0x50));
#if 1
  rx_upbod_chip1 = MS9601A_Up_Read(addr_up_0,0x51);
  rx_upbod_chip2 = MS9601A_Up_Read(addr_up_1,0x51);
  rx_upbod_chip3 = MS9601A_Up_Read(addr_up_2,0x51);

  rx_downbod_chip1 = MS9601A_Down_Read(addr_down_0,0x51);
  rx_downbod_chip2 = MS9601A_Down_Read(addr_down_1,0x51);
  rx_downbod_chip3 = MS9601A_Down_Read(addr_down_2,0x51);
#if 1
	if(twoTxPlugoutFlag0==1)
	{ if(d_port>5)//down-port6&7
    {
  		MS9601A_Down_Write(i2c_addr_pool[2],0x20, 0x03);
  		MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    else if(d_port>2)//down-port3&4&5
    {
      MS9601A_Down_Write(i2c_addr_pool[1],0x20, 0x03);
      MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    else //down-port0&1&2
    {
      MS9601A_Down_Write(i2c_addr_pool[0],0x20, 0x03);
      MS9601A_Down_Write(i2c_addr_pool[3],0x20, 0x03);
    }
    delay1ms(5);
		tx_hpd[0] = MS9601A_Down_Read(i2c_addr_pool[3],0x50);
		//printf("00NewPort->%u,rx_downbod->0x%02x,scan--txIN--tx_hpd[0]=%02x \r\n",NewPort,rx_downbod_chip1,tx_hpd[0]);
    /*当前激活口，上下板检测（单独检测）RX in----------------------*/
    if((rx_downbod_chip1&0x01)&&(d_port==0))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    else if((rx_downbod_chip1&0x02)&&(d_port==1))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    else if((rx_downbod_chip1&0x04)&&(d_port==2))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    else if((rx_downbod_chip2&0x01)&&(d_port==3))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    else if((rx_downbod_chip2&0x02)&&(d_port==4))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    else if((rx_downbod_chip2&0x04)&&(d_port==5))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    else if((rx_downbod_chip3&0x01)&&(d_port==6))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    else if((rx_downbod_chip3&0x02)&&(d_port==7))
      {if(tx_hpd[0]>>4&0X01)flg=1;}
    
    if(flg==1)
    { printf("2rx-in TX0 is insert \r\n");
    
      Lastport_tem =d_port;
      Global_portnum=d_port;
      lock_lastport=d_port;
      
      TxPlugoutFlag=0;
      twoTxPlugoutFlag0 = 0;          //检测到tx插入后置0 
      
      if(HpdMode==1)
			{	Set_ThisHpd_Low(d_port);}         
      else if(HpdMode==0&&(h_mode_flag>>d_port&0x01)==0)  
      {
        Set_ThisHpd_Low(d_port);      //H0模式下跳过这步，不拉低HPD。具体情况:当前口->其它口->当前口
        //printf("check01\r\n");
      }
      delay1ms(5);//20220801

      Switch_Port_GPIO(0,NewPort); 
      
      if(HpdMode==1){Force_Control_Rx0_Rx1_HPD(0,d_port);}                   
      else if(HpdMode==0&&(h_mode_flag>>d_port&0x01)==0)  
      {
  			Force_Control_Rx0_Rx1_HPD(0,d_port);//add 20220816
        h_mode_flag|=1<<d_port;       //H0模式下，端口切换后运行一次hdmi_rx_det()
        //printf("check02\r\n");
      }
          
#if (port2flash==0)
	    MemFlag=2;//当前口无RX in,不写flash
#endif
			Port_selected_state&=~0x00ff;
			Port_selected_state|=1<<port_count0;//设置下板select状态
      lock=1;
      flg=0;
      EQConfig=0;
      goto next;
    }
  }
#endif
  if(twoTxPlugoutFlag1==1)
  {
    //close rx-tx power for tx HPD detect
    if(up_port>5)//up-port6&7
    {
  		MS9601A_Up_Write(i2c_addr_pool[6],0x20, 0x03);
  		MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else if(up_port>2)//up-port3&4&5
    {
      MS9601A_Up_Write(i2c_addr_pool[5],0x20, 0x03);
      MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    else//up-port0&1&2
    {
      MS9601A_Up_Write(i2c_addr_pool[4],0x20, 0x03);
      MS9601A_Up_Write(i2c_addr_pool[7],0x20, 0x03);
    }
    delay1ms(5);
    tx_hpd[1] = MS9601A_Up_Read(i2c_addr_pool[7],0x50);
    
    //printf("11NewPort->%u,rx_downbod->0x%02x,scan--txIN--tx_hpd[1]=%02x \r\n",NewPort,rx_upbod_chip1,tx_hpd[1]);
    /*当前激活口，上下板检测（单独检测）RX in----------------------*/
    if((rx_upbod_chip1&0x01)&&(up_port==0))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
    else if((rx_upbod_chip1&0x02)&&(up_port==1))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
    else if((rx_upbod_chip1&0x04)&&(up_port==2))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
    else if((rx_upbod_chip2&0x01)&&(up_port==3))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
    else if((rx_upbod_chip2&0x02)&&(up_port==4))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
    else if((rx_upbod_chip2&0x04)&&(up_port==5))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
    else if((rx_upbod_chip3&0x01)&&(up_port==6))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
    else if((rx_upbod_chip3&0x02)&&(up_port==7))
      {if(tx_hpd[1]>>4&0X01)flg=2;}
		if(flg==2)
		{ printf("2rx-in TX1 is insert \r\n");
		
      Lastport_tem =d_port;
      Global_portnum=d_port;
      lock_lastport=d_port;

      TxPlugoutFlag=0;
	    twoTxPlugoutFlag1 = 0;               //检测到tx插入后置0 
	    
	    if(HpdMode==1)
			{	Set_ThisHpd_Low(up_port+8);}       
	    else if(HpdMode==0&&(h_mode_flag>>(up_port+8)&0x01)==0)  
	    {
	      Set_ThisHpd_Low(up_port+8);   //H0模式下跳过这步，不拉低HPD。具体情况:当前口->其它口->当前口
	      //printf("check11\r\n");
	    }
	    delay1ms(5);//20220801 

	    Switch_Port_GPIO(1,NewPort);
	    
	  	if(HpdMode==1){Force_Control_Rx0_Rx1_HPD(1,up_port); }                    
	    else if(HpdMode==0&&(h_mode_flag>>(up_port+8)&0x01)==0)  
	    {
	    	Force_Control_Rx0_Rx1_HPD(1,up_port);//add 20220816
	      h_mode_flag|=1<<(up_port+8);      //H0模式下，端口切换后运行一次hdmi_rx_det()
	      //printf("check12\r\n");
	    }
	    
	    #if (port2flash==0)
	    MemFlag=2;//当前口无RX in,不写flash
	    #endif
			
			Port_selected_state&=~0xff00;
			Port_selected_state|=1<<(port_count1+8);//设置上板select状态
      lock=1;
      flg=0;
      EQConfig=0;
next:
      printf("jump to next loop\r\n");
		}
#endif
  }
}

/*******************************************************************************
  * @FunctionName: pre_hdmi_rx_det
  * @Author:       ftansiX
  * @DateTime:     2022年8月1日T14:39:04+0800
  * @Purpose:      检测哪个RX in,curr_port(0~7)--->>>Global_portnum(0~15)双层
  * @param:        curr_port          
*******************************************************************************/
void pre_hdmi_rx_det(u8 curr_port)			
{
#if 1
  static u8 i=0,flg=0;
  u16 tempa,RX_IN_Status1;
	u8  j;
  u8  rx_upbod_chip1=0,rx_upbod_chip2=0,rx_upbod_chip3=0;
  u8  rx_downbod_chip1=0,rx_downbod_chip2=0,rx_downbod_chip3=0;
  port_count0=0xff;
  port_count1=0xff;
  rx_upbod_chip1 = MS9601A_Up_Read(addr_up_0,0x51);
  rx_upbod_chip2 = MS9601A_Up_Read(addr_up_1,0x51);
  rx_upbod_chip3 = MS9601A_Up_Read(addr_up_2,0x51);

  rx_downbod_chip1 = MS9601A_Down_Read(addr_down_0,0x51);
  rx_downbod_chip2 = MS9601A_Down_Read(addr_down_1,0x51);
  rx_downbod_chip3 = MS9601A_Down_Read(addr_down_2,0x51);
  //add20220808-->curr_port变化时，Global_portnum跟随变化。eg..curr_port=0时，Global_portnum取0或8
  if (curr_port==0)Global_portnum=0;
  if (curr_port==1)Global_portnum=1;
  if (curr_port==2)Global_portnum=2;
  if (curr_port==3)Global_portnum=3;
  if (curr_port==4)Global_portnum=4;
  if (curr_port==5)Global_portnum=5;
  if (curr_port==6)Global_portnum=6;
  if (curr_port==7)Global_portnum=7;

  RX_IN_Status1&=~(0x0007);
  RX_IN_Status1|=(u16)rx_downbod_chip1&0x0007;

  RX_IN_Status1&=~(0x0038);
  RX_IN_Status1|=(u16)rx_downbod_chip2<<3&0x0038;
  
  RX_IN_Status1&=~(0x00C0);
  RX_IN_Status1|=(u16)rx_downbod_chip3<<6&0x00C0;
  
  RX_IN_Status1&=~(0x0700);
  RX_IN_Status1|=(u16)rx_upbod_chip1<<8&0x0700;

  RX_IN_Status1&=~(0x3800);
  RX_IN_Status1|=(u16)rx_upbod_chip2<<11&0x3800;

  RX_IN_Status1&=~(0xC000);
  RX_IN_Status1|=(u16)rx_upbod_chip3<<14&0xC000;

  tempa^=RX_IN_Status1;

	for(j=0;j<16;j++)
	{
		if((h_mode_flag&(1<<j))&&(RX_IN_Status1&(1<<j)))h_mode_flag&=~(1<<j);
	}
  tempa=RX_IN_Status1;
#if 1
	//printf("chip1rx_value->0x%x,chip2rx_value->0x%x,chip3rx_value->0x%x  \r\n",rx_downbod_chip1,rx_downbod_chip2,rx_downbod_chip3);
  /******************************chip1-start******************************/
  /*-----------------------port0-start---------------------*/
  if ((rx_upbod_chip1 & 0x01)==0&&(curr_port==0)&&(input_flag&1<<curr_port+8)) //当前口重复插拔  
  {
    if(i==0)
    { PB03_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<8;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip1 & 0x01)&&(curr_port==0)) //上板Chip1-A 5Vin
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 8;
    Global_num |= 1<< curr_port+8;
    port_count1=0;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}//拔插后重新检测
  }
  /*--------------------*/
  if ((rx_downbod_chip1 & 0x01)==0&&(curr_port==0)&&(input_flag&1<<curr_port)) //当前口重复插拔  
  {
    if(i==0)
    {PA00_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<0;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip1 & 0x01)&&(curr_port==0))   //下板Chip1-A 5Vin
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 0;
    Global_num |= 1<< curr_port;
    port_count0=0;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*-----------------------port0-end---------------------*/
  
  /*-----------------------port1-start---------------------*/
  /*-------------------------------------------------------*/
  if ((rx_upbod_chip1 & 0x02)==0&&(curr_port==1)&&(input_flag&1<<curr_port+8))  
  {
    if(i==0)
    { PB04_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<9;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip1 & 0x02)&&(curr_port==1))
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 9;
    Global_num |= 1<< curr_port+8;
    port_count1=1;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*--------------------*/
  if ((rx_downbod_chip1 & 0x02)==0&&(curr_port==1)&&(input_flag&1<<curr_port)) 
  {
    if(i==0)
    { PA01_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<1;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip1 & 0x02)&&(curr_port==1))  
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 1;
    Global_num |= 1<< curr_port;
    port_count0=1;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*-----------------------port1-end---------------------*/
  
  /*-----------------------port2-start---------------------*/
  if ((rx_upbod_chip1 & 0x04)==0&&(curr_port==2)&&(input_flag&1<<curr_port+8)) 
  {
    if(i==0)
    { PB05_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<10;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip1 & 0x04)&&(curr_port==2)) 
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 10;
    Global_num |= 1<< curr_port+8;
    port_count1=2;
    if(redet_temp&(1<<Global_portnum)){ 
    flg=1;}
  }
  /*--------------------*/
  if ((rx_downbod_chip1 & 0x04)==0&&(curr_port==2)&&(input_flag&1<<curr_port))
  {
    if(i==0)
    { PA02_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<2;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip1 & 0x04)&&(curr_port==2))   
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 2;
    Global_num |= 1<< curr_port;
    port_count0=2;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*-----------------------port2-end---------------------*/
  /******************************chip1-end******************************/

  /******************************chip2-start******************************/
  /*-----------------------port0-start---------------------*/
  if ((rx_upbod_chip2 & 0x01)==0&&(curr_port==3)&&(input_flag&1<<curr_port+8)) 
  {
    if(i==0)
    { PB06_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<11;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip2 & 0x01)&&(curr_port==3))
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 11;
    Global_num |= 1<< curr_port+8;
    port_count1=3;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*--------------------*/
  if ((rx_downbod_chip2 & 0x01)==0&&(curr_port==3)&&(input_flag&1<<curr_port))
  {
    if(i==0)
    { PA03_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<3;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip2 & 0x01)&&(curr_port==3)) 
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 3;
    Global_num |= 1<< curr_port;
    port_count0=3;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*-----------------------port0-end---------------------*/
  
  /*-----------------------port1-start---------------------*/
  if ((rx_upbod_chip2 & 0x02)==0&&(curr_port==4)&&(input_flag&1<<curr_port+8))  
  {
    if(i==0)
    { PB07_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<12;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip2 & 0x02)&&(curr_port==4))
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 12;
    Global_num |= 1<< curr_port+8;
    port_count1=4;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*--------------------*/
  if ((rx_downbod_chip2 & 0x02)==0&&(curr_port==4)&&(input_flag&1<<curr_port))  
  {
    if(i==0)
    { PA04_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<4;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip2 & 0x02)&&(curr_port==4))  
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 4;
    Global_num |= 1<< curr_port;
    port_count0=4;
    if(redet_temp&(1<<Global_portnum)){ 
    flg=1;}
  }
  /*-----------------------port1-end---------------------*/
  
  /*-----------------------port2-start---------------------*/
  if ((rx_upbod_chip2 & 0x04)==0&&(curr_port==5)&&(input_flag&1<<curr_port+8))  
  {
    if(i==0)
    { PB08_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<13;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip2 & 0x04)&&(curr_port==5)) 
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 13;
    Global_num |= 1<< curr_port+8;
    port_count1=5;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*--------------------*/
  if ((rx_downbod_chip2 & 0x04)==0&&(curr_port==5)&&(input_flag&1<<curr_port))
  {
    if(i==0)
    { PA05_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<5;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip2 & 0x04)&&(curr_port==5))   
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 5;
    Global_num |= 1<< curr_port;
    port_count0=5;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*-----------------------port2-end---------------------*/
  /******************************chip2-end******************************/

  /******************************chip3-start******************************/
  /*-----------------------port0-start---------------------*/
  if ((rx_upbod_chip3 & 0x01)==0&&(curr_port==6)&&(input_flag&1<<curr_port+8)) 
  {
    if(i==0)
    { PB09_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<14;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip3 & 0x01)&&(curr_port==6)) 
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 14;
    Global_num |= 1<< curr_port+8;
    port_count1=6;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*--------------------*/
  if ((rx_downbod_chip3 & 0x01)==0&&(curr_port==6)&&(input_flag&1<<curr_port))
  {
    if(i==0)
    { PA06_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<6;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip3 & 0x01)&&(curr_port==6))   
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 6;
    Global_num |= 1<< curr_port;
    port_count0=6;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*-----------------------port0-end---------------------*/
  
  /*-----------------------port1-start---------------------*/
  if ((rx_upbod_chip3 & 0x02)==0&&(curr_port==7)&&(input_flag&1<<curr_port+8))   
  {
    if(i==0)
    { PB12_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port+8);
    redet_temp |=1<<15;
    Port_selected_state&=~(1<<curr_port+8);
  }
  else if ((rx_upbod_chip3 & 0x02)&&(curr_port==7))
  {
    i=0;
    input_flag |= 1<<curr_port+8;
    Global_portnum = 15;
    Global_num |= 1<< curr_port+8;
    port_count1=7;
    if(redet_temp&(1<<Global_portnum)){
    flg=1;}
  }
  /*--------------------*/
  if ((rx_downbod_chip3 & 0x02)==0&&(curr_port==7)&&(input_flag&1<<curr_port))  
  {
    if(i==0)
    { PA07_SETHIGH();
      i=1;
    }
    input_flag &= ~(1<<curr_port);
    redet_temp |=1<<7;
    Port_selected_state&=~(1<<curr_port);
  }
  else if ((rx_downbod_chip3 & 0x02)&&(curr_port==7))  
  {
    i=0;
    input_flag |= 1<<curr_port;
    Global_portnum = 7;
    Global_num |= 1<< curr_port;
    port_count0=7;
    if(redet_temp&(1<<Global_portnum)){
		flg=1;}
  }

  if(flg==1)
  { 
    if(lock)Global_portnum=lock_lastport;
    else
    {
      Tx_Hpd_reDetect=1;
      printf("Tx_Hpd_reDetect\r\n");
    }
    redet_temp =0;
    flg=0;
  }
  if(lock)Global_portnum=lock_lastport;
  /*-----------------------port1-end---------------------*/
  /******************************chip3-end******************************/
#endif
#endif
}

/*******************************************************************************
  * @FunctionName: Force_Control_Rx0_Rx1_HPD
  * @Author:       ftansiX
  * @DateTime:     2022年7月13日T16:28:13+0800
  * @Purpose:      设置port的 hpd为高
  * @param:        board: A->下板0 , B->上板1             
  * @param:        port               
*******************************************************************************/
void Force_Control_Rx0_Rx1_HPD(u8 board,u8 port)
{	if(board)
  { printf("BOARD B \r\n");
  	if(port == 0)
  	{	PB03_SETLOW();
  	}
  	else if(port == 1)
  	{ PB04_SETLOW();
  	}
  	else if(port == 2)
  	{	PB05_SETLOW();
  	}
  	else if(port == 3)
  	{	PB06_SETLOW();
  	}	
  	else if(port == 4)
  	{ PB07_SETLOW();
  	}
  	else if(port == 5)
  	{	PB08_SETLOW();
  	}
  	else if(port == 6)
  	{	PB09_SETLOW();
  	}
  	else if(port == 7)
  	{	PB12_SETLOW();
  	}	
  } 
  else
  { printf("BOARD A \r\n");
  	if(port == 0)
  	{	PA00_SETLOW();
  	}
  	else if(port == 1)
  	{ PA01_SETLOW();
  	}
  	else if(port == 2)
  	{	PA02_SETLOW();
  	}
  	else if(port == 3)
  	{	PA03_SETLOW();
  	}	
  	else if(port == 4)
  	{ PA04_SETLOW();
  	}
  	else if(port == 5)
  	{	PA05_SETLOW();
  	}
  	else if(port == 6)
  	{	PA06_SETLOW();
  	}
  	else if(port == 7)
  	{	PA07_SETLOW();
  	}	
  }
}
/*******************************************************************************
  * @FunctionName: Set_OtherHpd_Low
  * @Author:       ftansiX
  * @DateTime:     2022年7月14日T13:32:19+0800
  * @Purpose:      除当前port,其余port——hpd全部置低（H1模式）,仅限单层，上下层联动
  * @param:        Glo_num          
*******************************************************************************/
void Set_OtherHpd_Low(u8 port)
{ 
	if(port==0);
  else  
  { PA00_SETHIGH();
    PB03_SETHIGH();
  }
  
  if(port==1);
  else
  { PA01_SETHIGH();
    PB04_SETHIGH();
  }
  if(port==2);
  else
  { PA02_SETHIGH();
    PB05_SETHIGH();
  }
  
  if(port==3);
  else
  { PA03_SETHIGH();
    PB06_SETHIGH();
  }
  
  if(port==4);
  else
  { PA04_SETHIGH();
    PB07_SETHIGH();
  }
  
  if(port==5);
  else
  { PA05_SETHIGH();
    PB08_SETHIGH();
  }
  
  if(port==6);
  else
  { PA06_SETHIGH();
    PB09_SETHIGH();
  }
  
  if(port==7);
  else
  { PA07_SETHIGH();
    PB12_SETHIGH();
  }
}
extern u8 hpd_dont_low;
void singleSet_OtherHpd_Low(u8 port)
{ 
	u8 tempp=0xff;
//	tempp=port;
//	if(tempp<8)
//	{
//		tempp = port+8;
//		if(tempp==8)tempp=15;
//		else tempp=port-1;
//	}
//	else if(tempp<16)
//	{
//		tempp = port-8;
//		if(tempp==0)tempp=7;
//		else tempp=port-1;
//	}

	printf("hpd_dont_low=%u  port=%u--\r\n",hpd_dont_low,port);
	if(port==0 ||tempp==0 ||hpd_dont_low==0);
  else  
  { PA00_SETHIGH();
  }
  
  if(port==1 ||tempp==1 ||hpd_dont_low==1);
  else
  { PA01_SETHIGH();
  }
  if(port==2 ||tempp==2 ||hpd_dont_low==2);
  else
  { PA02_SETHIGH();
  }
  
  if(port==3 ||tempp==3 ||hpd_dont_low==3);
  else
  { PA03_SETHIGH();
  }
  
  if(port==4 ||tempp==4 ||hpd_dont_low==4);
  else
  { PA04_SETHIGH();
  }
  
  if(port==5 ||tempp==5 ||hpd_dont_low==5);
  else
  { PA05_SETHIGH();
  }
  
  if(port==6 ||tempp==6 ||hpd_dont_low==6);
  else
  { PA06_SETHIGH();
  }
  
  if(port==7 ||tempp==7 ||hpd_dont_low==7);
  else
  { PA07_SETHIGH();
  }

	if(port==8 ||tempp==8 ||hpd_dont_low==8);
  else  
  { 
    PB03_SETHIGH();
  }
  
  if(port==9 ||tempp==9 ||hpd_dont_low==9);
  else
  { 
    PB04_SETHIGH();
  }
  if(port==10 ||tempp==10 ||hpd_dont_low==10);
  else
  { 
    PB05_SETHIGH();
  }
  
  if(port==11 ||tempp==11 ||hpd_dont_low==11);
  else
  { 
    PB06_SETHIGH();
  }
  
  if(port==12 ||tempp==12 ||hpd_dont_low==12);
  else 
  { 
    PB07_SETHIGH();
  }
  
  if(port==13 ||tempp==13 ||hpd_dont_low==13);
  else
  { 
    PB08_SETHIGH();
  }
  
  if(port==14 ||tempp==14 ||hpd_dont_low==14);
  else
  { 
    PB09_SETHIGH();
  }
  
  if(port==15 ||tempp==15 ||hpd_dont_low==15);
  else
  { 
    PB12_SETHIGH();
  }
	
//	if(port==0 ||tempp==0);
//  else  
//  { PA00_SETHIGH();
//    PB03_SETHIGH();
//  }
//  
//  if(port==1 ||tempp==1);
//  else
//  { PA01_SETHIGH();
//    PB04_SETHIGH();
//  }
//  if(port==2 ||tempp==2);
//  else
//  { PA02_SETHIGH();
//    PB05_SETHIGH();
//  }
//  
//  if(port==3 ||tempp==3);
//  else
//  { PA03_SETHIGH();
//    PB06_SETHIGH();
//  }
//  
//  if(port==4 ||tempp==4);
//  else
//  { PA04_SETHIGH();
//    PB07_SETHIGH();
//  }
//  
//  if(port==5 ||tempp==5);
//  else
//  { PA05_SETHIGH();
//    PB08_SETHIGH();
//  }
//  
//  if(port==6 ||tempp==6);
//  else
//  { PA06_SETHIGH();
//    PB09_SETHIGH();
//  }
//  
//  if(port==7 ||tempp==7);
//  else
//  { PA07_SETHIGH();
//    PB12_SETHIGH();
//  }
}

/*******************************************************************************
  * @FunctionName: Set_ThisHpd_Low
  * @Author:       ftansiX
  * @DateTime:     2022年7月29日T15:56:06+0800
  * @Purpose:      将当前口置低
  * @param:        num            
*******************************************************************************/
extern u16 Global_num;
void Set_ThisHpd_Low(u8 num)
{ switch(num)
  { case 0:
      if(Global_num&1<<num)PA00_SETHIGH();
      break;
    case 1:
      if(Global_num&1<<num)PA01_SETHIGH();
      break;
    case 2:
      if(Global_num&1<<num)PA02_SETHIGH();
      break;
    case 3:
      if(Global_num&1<<num)PA03_SETHIGH();
      break;
    case 4:
      if(Global_num&1<<num)PA04_SETHIGH();
      break;
    case 5:
      if(Global_num&1<<num)PA05_SETHIGH();
      break;
    case 6:
      if(Global_num&1<<num)PA06_SETHIGH();
      break;
    case 7:
      if(Global_num&1<<num)PA07_SETHIGH();
      break;
    
    case 8:
      if(Global_num&1<<num)PB03_SETHIGH();
      break;
    case 9:
      if(Global_num&1<<num)PB04_SETHIGH();
      break;
    case 10:
      if(Global_num&1<<num)PB05_SETHIGH();
      break;
    case 11:
      if(Global_num&1<<num)PB06_SETHIGH();
      break;
    case 12:
      if(Global_num&1<<num)PB07_SETHIGH();
      break;
    case 13:
      if(Global_num&1<<num)PB08_SETHIGH();
      break;
    case 14:
      if(Global_num&1<<num)PB09_SETHIGH();
      break;
    case 15:
      if(Global_num&1<<num)PB12_SETHIGH();
      break;
  }
}


