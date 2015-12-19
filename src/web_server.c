/******************** (C) COPYRIGHT 2011 ����Ƕ��ʽ���������� ********************
 * �ļ���  ��simple_server.c
 * ����    ��web�������Ӧ�ú�����
 *          
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ� ------------------------------------
 *           |PB13         ��ENC28J60-INT (û�õ�)|
 *           |PA6-SPI1-MISO��ENC28J60-SO          |
 *           |PA7-SPI1-MOSI��ENC28J60-SI          |
 *           |PA5-SPI1-SCK ��ENC28J60-SCK         |
 *           |PA4-SPI1-NSS ��ENC28J60-CS          |
 *           |PE1          ��ENC28J60-RST (û��)  |
 *            ------------------------------------
 * ��汾  ��ST3.0.0
 *
 * ����    ��fire  QQ: 313303034
 * ����    ��firestm32.blog.chinaunix.net
**********************************************************************************/
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "web_server.h"
#include <string.h>
#include "integer.h"

//#include "led.h" 			// LED �������ͷ�ļ�

/* mac��ַ��ip��ַ�ھ������ڱ���Ψһ������������������ͻ���������Ӳ��ɹ� */
static unsigned char mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24};
static unsigned char myip[4] = {192,168,1,98};

/* ip��ַ(������DNS�����֣������DNS�������Ļ�)��ip��ַ������"/"��β */
//static char baseurl[]="http://192.168.1.1/";

/* tcp/www�����˿ںţ���ΧΪ:1-254 */
static unsigned int mywwwport =80; 

/* udp �����˿ںţ�������(������)�˿ں� */
static unsigned int myudpport =1200; 

/* �������ݻ����� */
#define BUFFER_SIZE 1500
unsigned char buf[1501];

/* ����,���ܴ���9���ַ�(ֻ�������ǰ5λ�ᱻ���)��(�ַ��޶�Ϊ��a-z,0-9) */
static char password[]="123456"; 

unsigned char indarray[20]="";
unsigned int lenind=0;
unsigned char glflag=0;


//�ⲿ����������
extern int writelog(unsigned char * filename,unsigned char * filecontent,unsigned char lencont,unsigned char sflag);
extern int printallfile(unsigned char * filename );


// �ⲿ����������
extern unsigned char gflag_send;
extern DWORD send_count;
/*
 * ��������verify_password
 * ����  ��ȷ������
 * ����  ��str
 * ���  ����
 * ����  ��
 */ 
unsigned char verify_password(char *str)
{
    // the first characters of the received string are
    // a simple password/cookie:
    if (strncmp(password,str,5)==0)
    {
        return(1);
    }
    return(0);
}

/*
 * ��������analyse_get_url
 * ����  ��takes a string of the form password/commandNumber and analyse it
 * ����  ��str
 * ���  ��-1 invalid password, otherwise command number
 *         -2	no command given but password valid
 * ����  ��
 */
unsigned char analyse_get_url(char *str)
{
    unsigned char i=0;
    //if (verify_password(str)==0)
    //{
    //    return(-1);
    //}
    // find first "/"
    // passw not longer than 9 char:
    while(*str && i<10 && *str >',' && *str<'{')
    {
        if (*str=='/')
        {
            str++;
            break;
        }
        i++;
        str++;
    }
    if (*str < 0x3a && *str > 0x2f)
    {
        // is a ASCII number, return it
        return(*str-0x30);
    }
    return(-2);
}

/*
 * ��������print_webpage
 * ����  ��������д��tcp�ķ��ͻ�����(��ʵ��һ����ҳ)
 * ����  ��-buf
 *         -on_off
 * ���  ����
 * ����  ��-plen
 */
unsigned int print_webpage(unsigned char *buf,unsigned char on_off)
{
    unsigned int plen;
		/* �½�һ����ҳ�������½�һ���ļ�һ�� */
    plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
	
		/* ����ҳ��������д��Ϣ */
    plen=fill_tcp_data_p(buf,plen,PSTR("<center><p> ��ǰU��״̬: <p>"));

    

    plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��1��:"));
	  plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��2��:"));
	  plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��3��:"));
	  plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��4��:"));
	  plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��5��:"));
	  plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��6��:"));
	  plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��7��:"));
	  plen=fill_tcp_data_p(buf,plen,PSTR(" <br>λ��8��:"));

    
    
    plen=fill_tcp_data_p(buf,plen,PSTR("<hr><br>****************^_^ ��ӭʹ�ù���������ҳ����ϵͳ ^_^****************\n</center>"));
    //plen=fill_tcp_data_p(buf,plen,PSTR("����һ�� web ���Գ���"));
    
    return(plen);
}
/*
 * ��������SetIpMac
 * ����  ������IP��ַ��Mac��ַ
 * ����  ����
 * ���  ����
 * ����  ��-0 ���гɹ�
 * Ӧ��  ��1 ��PC����DOS�������룺 ping 192.168.1.15 (���ܷ�pingͨ)
 *         2 ��IE����������룺http://192.168.1.15/123456 ������һ����ҳ��ͨ����ҳ
 *           �е�������Կ��ƿ������е�LED������
 */

void SetIpMac()
{
	/* ��ʼ�� enc28j60 ��MAC��ַ(�����ַ),�����������Ҫ����һ�� */
  enc28j60Init(mymac);

	/* PHY LED ����,LED����ָʾͨ�ŵ�״̬ */	
  enc28j60PhyWrite(PHLCON,0x476);	
	
	/* ��enc28j60�������ŵ�ʱ�������Ϊ��from 6.25MHz to 12.5MHz(�����̸�����NC,û�õ�) */	
  //enc28j60clkout(2);    
  
	/* ��ʼ����̫�� IP �� */
	init_ip_arp_udp_tcp(mymac,myip,mywwwport);
	
}

/*
 * ��������SendTcp
 * ����  ����������ϴ���һ��web��������ͨ��web��������������ƿ������ϵ�LED������
 * ����  ����
 * ���  ����
 * ����  ��-0 ���гɹ�
 */
void SendTcp(unsigned int plen)
{
              make_tcp_ack_from_any(buf);       // send ack for http get
              make_tcp_ack_with_data(buf,plen); // send data
	
}



/*
 * ��������Web_Server
 * ����  ����������ϴ���һ��web��������ͨ��web��������������ƿ������ϵ�LED������
 * ����  ����
 * ���  ����
 * ����  ��-0 ���гɹ�
 * Ӧ��  ��1 ��PC����DOS�������룺 ping 192.168.1.15 (���ܷ�pingͨ)
 *         2 ��IE����������룺http://192.168.1.15/123456 ������һ����ҳ��ͨ����ҳ
 *           �е�������Կ��ƿ������е�LED������
 */
int Web_Server(void)
{   
	unsigned int plen, i1 = 0;
	unsigned int dat_p;
	unsigned char  *buf1;
	unsigned int payloadlen = 0;
   
			// get the next new packet:			
      plen = enc28j60PacketReceive(BUFFER_SIZE, buf); 	
      
      // plen will ne unequal to zero if there is a valid packet (without crc error)			
      if(plen==0)
      {
          return (0);
      }

      // arp is broadcast if unknown but a host may also
      // verify the mac address by sending it to 
      // a unicast address.		     
      if(eth_type_is_arp_and_my_ip(buf,plen))
      {
          make_arp_answer_from_request(buf);          
          return (0);
      }
      
      // check if ip packets are for us:			
      if(eth_type_is_ip_and_my_ip(buf,plen)==0) 
      {
         return (0);
      }      
      
      if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
      {
          // a ping packet, let's send pong  DOS �µ� ping �����		 
          make_echo_reply_from_request(buf, plen);          
          return (0);
      }

/*-----------------tcp port www start, compare only the lower byte-----------------------------------*/
      if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==mywwwport)
      {
          if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
          {
              make_tcp_synack_from_syn(buf);
              // make_tcp_synack_from_syn does already send the syn,ack
             return (0);
          }
          if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
          {
              init_len_info(buf); // init some data structures
              // we can possibly have no data, just ack:
              dat_p=get_tcp_data_pointer();
              if (dat_p==0)
              {
                  if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
                  {
                      // finack, answer with ack
                      make_tcp_ack_from_any(buf);
                  }
                  // just an ack with no data, wait for next packet
                 return (0);
              }
							if (strncmp("user",(char *)&(buf[dat_p]),4)==0)
              {
								 
								  lenind=buf[dat_p+4]*256;
								  lenind+=buf[dat_p+5];
								  memcpy(indarray,buf+dat_p+4+2,lenind);
								  // glflag=1;   //д���û���
								  writelog("0:/test.txt",indarray,lenind,1);
								  indarray[lenind]='\0';
								  plen=fill_tcp_data_p(buf,0,indarray);
                  SendTcp(plen);
								  return (0);
              }
							if (strncmp("logr",(char *)&(buf[dat_p]),4)==0) //��ȡlog��־
              {
                               
								
								  printallfile("0:/test.txt");
								  return (0);
              }
							if (strncmp("logs",(char *)&(buf[dat_p]),4)==0) //���ζ�ȡlog��־
              {							
								  gflag_send=0;
								  send_count=0;
								  return (0);
              }
             
             
          //  plen=print_webpage(buf,(i));     
          //  make_tcp_ack_from_any(buf);       // send ack for http get
          //  make_tcp_ack_with_data(buf,plen); // send data
            return (0);
          }
      }
/*-------------------------------------- tcp port www end ---------------------------------------*/

/*--------------------- udp start, we listen on udp port 1200=0x4B0 -----------------------------*/
      if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==4&&buf[UDP_DST_PORT_L_P]==0xb0)
      {
          payloadlen=	  buf[UDP_LEN_H_P];
          payloadlen=payloadlen<<8;
          payloadlen=(payloadlen+buf[UDP_LEN_L_P])-UDP_HEADER_LEN;
          //payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
          
          //ANSWER:
          //while(1){
          for(i1=0; i1<payloadlen; i1++) buf1[i1]=buf[UDP_DATA_P+i1];
				//printf("%s",buf);
          
          make_udp_reply_from_request(buf,buf1,payloadlen,myudpport);          
          //}
      }
/*----------------------------------------udp end -----------------------------------------------*/
  
          return (0);
}

