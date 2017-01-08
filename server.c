//
//  server.c
//  
//
//  Created by Madhav on 20/09/16.
//
//

//#include "server.h"
#include "commands.h"
#include <stdio.h>
#include <string.h>
//#include "client.h"
//#include "server.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <linux/if_link.h>

IPLIST iplist1[10];
#define MAX_CONN 4
#define MESSAGE_SIZE 1500
#define STDIN 0



void reset_status()
{
for (int i=1;i<=10;i++)
{iplist1[i].status=0;
iplist1[i].id=0;
}           
}



void ip_hostname(char *ip,char *hostname)
{
struct hostent *he;


//gets(ip);
//puts(ip);
//char hostname[50];

struct in_addr ipv4addr;

inet_pton(AF_INET,ip, &ipv4addr);
he = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);
if(he==NULL)
{
strcpy(hostname,"unknownhost");

}
else
{
strcpy(hostname,he->h_name);

}
}

int  reorderids1()
{     
  int count=1;

  for (int i=1;i<=10;i++)

  { 
        if(iplist1[i].status==1)
        {
           iplist1[i].id=count;
           count++;
          


        }    
  }
  return 1;


}

int  append_to_iplist1(char hostname[255],char ipaddress[255],int filedesc,int port)
{  int flag=0;
  for (int i=1;i<=10;i++)
  { 
        if(iplist1[i].status==0)
        {
        	strcpy(iplist1[i].hostname,hostname);
        	strcpy(iplist1[i].ipaddress,ipaddress);
        	iplist1[i].filedesc=filedesc;
        	iplist1[i].listenport=port;
        	iplist1[i].status=1;
          flag=1;


        } 
        if (flag==1)
        break;   
  }

   
   reorderids1();
   return 1;
}

void server_handle_disconnet1(int sel_sock)
{
   for (int i=1;i<=10;i++)

  { 
        if(iplist1[i].filedesc==sel_sock)

        {

        	iplist1[i].status=0;
          printf("IP Address:%s,Port:%d \n",iplist1[i].ipaddress,iplist1[i].listenport);


        }    
}
reorderids1();

}

void get_ip_list1()

{
	printf("%-6s%-36s%-21s%-9s \n", "ID", "HOSTNAME", "IP ADDRESS", "PORT");
        for (int i=1;i<=10;i++)
   { 
        if(iplist1[i].status==1)
        {
         
         printf("%-6d%-36s%-21s%-9d \n",iplist1[i].id,iplist1[i].hostname,iplist1[i].ipaddress,iplist1[i].listenport);
        

        }

            
  }


}



void get_help1()
{

    printf("Options available on this server \n");
    printf("HELP    Display's the commands supported on this clients \n");
    printf("LIST    Display's list of clients registered on this server\n");
    printf("CREATOR Display's creators full name, UBIT name and UB email address\n");
    printf("DIPSLAY Display's IP address and listening port of this process\n");
    printf("QUIT    Shutdowns all the socket connections associated with server\n");
    

}

void get_creator1()

{
printf("NAME:%s\tUBITname:%s\tUBEmail:%s \n","Hema Madhav Jakkampudi","hemamadh","hemamadh@buffalo.edu");
}

void get_ipaddr1(int port)
{    
    char tempip[32];
    struct ifaddrs *interfaceaddress,*tempaddr;
    char localip[32];
    getifaddrs(&interfaceaddress);
    for (tempaddr=interfaceaddress;tempaddr!=NULL;tempaddr=tempaddr->ifa_next) {
    if(tempaddr->ifa_addr->sa_family == AF_INET) {
    getnameinfo(tempaddr->ifa_addr, sizeof(struct sockaddr_in),tempip,32, NULL, 0,NI_NUMERICHOST);
    if(strcmp(tempip,"127.0.0.1")!=0)
    {
        if(strstr(tempip,"128"))
    {
    strcpy(localip,tempip);
    }       
     }
 }
    }
    printf("Server is running at %s:%d\n",localip,port);
}





void send_to_all_clients1(int connect_count)
{   

	int temp_count=0;
  char msg[300];
  char count[4];
  char temp_port[6];
  sprintf(count,"%d",connect_count);
  int byte_count;
for(int j=1;j<=10;j++)
{  

  if(iplist1[j].status==1)
  {
     int temp_filedesc=iplist1[j].filedesc;
     if(send(temp_filedesc,count,sizeof(count),0)<0)
     {
       printf("%s\n","send failed for count" );
     }

  
        
    for(int i=1;i<=10;i++)
    {
        if(iplist1[i].status==1)
        { memset(msg,0,sizeof(msg));
          sprintf(temp_port,"%d",iplist1[i].listenport);
          strcat(msg,"dummy");
          strcat(msg,"#");
        	strcat(msg,iplist1[i].hostname);
          strcat(msg,"#");
        	strcat(msg,temp_port);
          strcat(msg,"#");
        	strcat(msg,iplist1[i].ipaddress);
          if((byte_count=send(temp_filedesc,msg,sizeof(msg), 0))<0)
          {
            printf("%s\n","send failed to transmit structure" );
          }
          //printf("%d\n",byte_count);
          //printf("%s\n",msg);
        }

  
  }
}

}

printf("list successfully sent to all the available clients\n");
}
       
    
    

    




int get_max_scokFD1()
{

  int max=0;
 for(int i=1;i<=10;i++)
    {
        if(iplist1[i].status==1)
        {
          if(iplist1[i].filedesc > max)
            max=iplist1[i].filedesc;
        }
       
    }
      return max;
}


int initServer(int port)
{

int socket1,enable_reuse=1,fdmax,connect_count=0;
struct sockaddr_in server_addr,client_addr,sa;
char message[MESSAGE_SIZE];char cmd_buf[15];
char clientIP[32],host_name[50];
server_addr.sin_family=AF_INET;
server_addr.sin_addr.s_addr=INADDR_ANY; 
server_addr.sin_port=htons(port);

//{
//  perror("inet_pton");
//}
//printf("%d\n",port);
reset_status();
socket1=socket(AF_INET,SOCK_STREAM,0);

if(socket1<0)
   {
       perror("Server:Socket() init failed");
       return -1;
   }


//memset(&server_addr,0,sizeof(server_addr));

if(bind(socket1,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)
{
    perror("Bind() failed");
    close(socket1);
    return -1;

}
if(setsockopt(socket1,SOL_SOCKET,SO_REUSEADDR,&enable_reuse,sizeof(int))<0)
{
  perror("Server:Setsocketopt() init failed");
  return -1;
}
//int enable_reuse=1;


if((listen(socket1,MAX_CONN))==0)
{
printf("%s\n","listening" );
}
else
printf("%s\n","Error with listening socket");

fd_set master_read,read_fds,write_fds;
FD_ZERO(&read_fds);
FD_ZERO(&write_fds);
FD_ZERO(&master_read);
FD_SET(STDIN,&master_read);
FD_SET(socket1,&master_read);
fdmax=socket1;
int recv_bytes;
int sel_sock;

printf("server is successfully up and running\n");
printf("type help to know about functionality\n");

for(;;)
{
      //FD_SET(STDIN,&read_fds);
      //FD_SET(socket1,&read_fds);
     //printf("%s\n","hello" );
         read_fds = master_read;
		if (select(fdmax+1, &read_fds,NULL, NULL, NULL)<0)
		{
			perror("select() failed");
			continue;
		}

       

    for (sel_sock=0;sel_sock<=fdmax;sel_sock++)

    {    //printf("%s\n","hi" );


       	if(FD_ISSET(sel_sock,&read_fds))
           
        {   
       		if(sel_sock==socket1)

       	    {	
       	      
       	        	
       	      int client_length = sizeof(client_addr);
              printf("accepting new connections\n");
       	      int client_socket=accept(socket1,(struct sockaddr*)&client_addr,(socklen_t*)&client_length);
              if(connect_count==MAX_CONN)
              {
                printf("Maximum Connection limit on this server is:%d\n",MAX_CONN);
                close(client_socket);
              }
              else
               {
   						if (client_socket<0)
   						{
   							perror("accept() failed");
   						}
   						connect_count++;
   						setsockopt(client_socket,SOL_SOCKET,SO_REUSEADDR,&enable_reuse,sizeof(int));
              //int recvport = ntohs (((struct sockaddr_in*)&client_addr)->sin_port);
              //printf("%d\n",recvport);

   						memset(message,0,MESSAGE_SIZE);


   						recv_bytes = recv(client_socket,message,MESSAGE_SIZE,0);
              if(recv_bytes<0)
              {
                printf("%s\n","Unable to receive ports");
              }

               //printf("%s\n",message);

   						printf("New client has been accepted by the server\n");
   						FD_SET(client_socket,&master_read);
              //printf("%s\n","AA" );

   						if(fdmax<client_socket)
   						{
   								fdmax=client_socket;
   						}
              //printf("%d\n",fdmax );

                if(inet_ntop(AF_INET, &(client_addr.sin_addr),clientIP,32)<0)
                {
                  printf("inet_ntop() failed to convert client IP\n");
                }
                
                ip_hostname(clientIP,host_name);
                
                printf("connection from client Hostname:%s,ipaddress:%s\n",host_name,clientIP);
   							append_to_iplist1(host_name,clientIP,client_socket,atoi(message));// append_to_iplist(char hostname,char ipaddress,int filedesc,int port)
                
   							printf("New client has been added to list\n");
                
                //get_ip_list1();
   							send_to_all_clients1(connect_count);
                printf("Clients are updated with IP list\n");
             
   							
       	        
       	        }

       	        }

            else if(sel_sock==0)

              {
              int byte_count=read(sel_sock , cmd_buf , sizeof(cmd_buf));

              //printf("%d",byte_count);

              if ( byte_count >= 0 )
              {
                  cmd_buf[ byte_count-1] = '\0';
              }
              
                for(int i = 0;cmd_buf[i] != '\0'; i++)
                {
                cmd_buf[i] = tolower(cmd_buf[i]);

                }
                //printf("%s",cmd_buf);
                if(strcmp(cmd_buf,"help")==0)
                {
                  get_help1();
                }
                else if(strcmp(cmd_buf,"creator")==0)
                {
                  get_creator1();
                }
                else if(strcmp(cmd_buf,"display")==0)
                {
                  get_ipaddr1(port);
                }
                else if(strcmp(cmd_buf,"list")==0)
                {
                  get_ip_list1();
                }
                else if(strcmp(cmd_buf,"quit")==0)
                {
                  
                for (int i=1;i<=10;i++)
                {    
                  if(iplist1[i].status==1)
                  {
                    if(shutdown(iplist1[i].filedesc,SHUT_RDWR)<0)
                    {
                      printf("Unable to shutdown the socket\n");

                      } close(socket1);
                        close(iplist1[i].filedesc);
                        FD_CLR(iplist1[i].filedesc,&master_read);

                      }

                    }
                    printf("Closing all the connections and quiting.\n");
                    exit(0);                  

                }
                else
                {
                  printf("The command you typed is not supported on this server\n");
                  //continue;
                }

                }

                else

                {
                    memset(message,0,MESSAGE_SIZE);
                    //printf("%s\n", "Else block");
                    if ((recv_bytes = recv(sel_sock, message,MESSAGE_SIZE, 0)) <= 0)
                    {
                    if(recv_bytes==0)
                    {

                    	connect_count--;
                      FD_CLR(sel_sock,&master_read);
                      close(sel_sock);
                     	printf("Client terminated connection\n");
                      server_handle_disconnet1(sel_sock);
                      fdmax=get_max_scokFD1();

                     	if(connect_count!=0)
                     	{
                     
                     	send_to_all_clients1(connect_count);

                      reorderids1();
                      
                      }
                      else
                        fdmax=socket1;
                    
                    }
                  }

                }

                break;
        }
    

    	

    }

}


}

















