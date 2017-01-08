//
//  client.c
//  
//
//  Created by Madhav on 10/4/16.
//
//

//#include "client.h"
#include "commands.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
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
//#include <linux/if_link.h>
#include <unistd.h>
#define MAX_CONN 4
#define MESSAGE_SIZE 1500
IPLIST iplist[10];
IPLIST server_list[10];
char localip[32];
fd_set read_fds,write_fds,client_fds_r,client_fds_w;
int receiving_file_size=0;
int receiving_count=0;



int server_socket;

void iptohostname(char *ip,char *hostname)
{
struct hostent *he;
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


int reset_status_peer()
{
for (int i=1;i<=9;i++)
{
  iplist[i].status=0;
  iplist[i].id=0;
 } 
 return 1;          
}


int reset_status_server()
{
for (int i=1;i<=9;i++)
{ server_list[i].status=0;
 server_list[i].id=0;
 } 
 return 1;          
}





void reorderids()
{     
  int count=2;

  for (int i=2;i<=9;i++)

  { 
        if(iplist[i].status==1)
        {
           iplist[i].id=count;
           count++;
          


        }    
  }


}


void append_to_iplist(char hostname[255],char ipaddress[255],int filedesc,int port)
{
  for (int i=2;i<=9;i++)
  {      int flag=0;
        if(iplist[i].status==0)
        {
          strcpy(iplist[i].hostname,hostname);
          strcpy(iplist[i].ipaddress,ipaddress);
          iplist[i].filedesc=filedesc;
          iplist[i].listenport=port;
          iplist[i].status=1;
          flag=1;


        }    
        if(flag==1)
          break;
  }

   reorderids();
}

void peer_handle_disconnet(int sel_sock)
{
   for (int i=1;i<=9;i++)

  { 
        if(iplist[i].filedesc==sel_sock)

        {

          iplist[i].status=0;


        } 
        

}
reorderids();

}


void handle_server_update_list(int number,int sel_sock)
{  
   char buffer[300];
   char port[6];
   int byte_count;
   char dummy[7];
   for (int i=1;i<=number;i++)
   { 

    memset(buffer,0,sizeof(buffer));
    if((byte_count=recv(sel_sock,buffer,sizeof(buffer),0))<0)
    {
      printf("Unable to Receive server update list\n");
    }
    strcpy(dummy,strtok(buffer , "#"));
    strcpy(server_list[i].hostname ,strtok(NULL , "#"));
    strcpy(port, strtok(NULL, "#"));
    strcpy(server_list[i].ipaddress, strtok(NULL, "#"));
    server_list[i].listenport=atoi(port);
    server_list[i].status=1;

    }
        

            
  }


void reset_update_list_server()
{
for (int i=1;i<=9;i++)
   { 
        server_list[i].status=0;
        

            
  }

}

void reset_update_list_client()
{
for (int i=1;i<=9;i++)
   { 
        iplist[i].status=0;
        

            
  }

}

void display_update_server_list()
{


  printf("%-36s%-21s%-9s\n", "HOSTNAME", "IP ADDRESS", "PORT");
        for (int i=1;i<=9;i++)
   { 
        if(server_list[i].status==1)
        {
         
         printf("%-36s%-21s%-9d\n",server_list[i].hostname,server_list[i].ipaddress,server_list[i].listenport);
        

        }

            
  }
}

void get_ip_list()

{
  printf("%-6s%-36s%-21s%-9s\n", "ID", "HOSTNAME", "IP ADDRESS", "PORT");
        for (int i=1;i<=9;i++)
   { 
        if(iplist[i].status==1)
        {
         
         printf("%-6d%-36s%-21s%-9d\n",iplist[i].id,iplist[i].hostname,iplist[i].ipaddress,iplist[i].listenport);
        

        }

            
  }


}



void get_help()
{

    printf("Options available on this Clients.*Note that there is Maximum connection Limitation.\n");
    printf("LIST      Display's list of clients registered on this server\n");
    printf("CREATOR   Display's creators full name, UBIT name and UB email address\n");
    printf("DIPSLAY   Display's IP Address and port of this process\n");
    printf("HELP      Display's the commands supported on this clients \n");
    printf("TERMINATE <ID> Closes the connections with the peer ID specified from the list \n");
    printf("REGISTER  <SERVERIP> <Port> Registers this Client on the IP address and port of the SERVER Specified.\n");
    printf("QUIT     Closes all the connections this client is connected to and exits the program \n");
    printf("CONNECT <IP/Hostname> <Port> - Connects to a client already registered with the server\n");
    printf("PUT <CONNECTION ID> <FILE NAME> - Uploads the file to the specified peer\n");
    printf("GET <CONNECTION ID> <FILE NAME> - Downloads the file to the specified peer\n");
}

void get_creator()

{
printf("NAME:%s\tUBITname:%s\tUBEmail:%s \n","Hema Madhav Jakkampudi","hemamadh","hemamadh@buffalo.edu");
}

void get_ipaddr(int port)
{

{
    char tempip[32];
    struct ifaddrs *interfaceaddress,*tempaddr;
    //char localip[IPV4];
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



}


int get_max_scokFD()
{

  int max=0;
 for(int i=1;i<=9;i++)
    {
        if(iplist[i].status!=0)
        {
          if(iplist[i].filedesc > max)
            max=iplist[i].filedesc;
        }
       
    }
      return max;
}


int handle_peer_upload(int socket,char filename[50])
{
      char sending_buffer[256];
      //char *err="File Doesn't Exist";
      int sending_file_size=0;
      int sending_count=0;
      int file_read_bytes;
      int sending_bytes;
      
      int temp_file_desc=open(filename, O_RDONLY);

     
        while( (file_read_bytes = read(temp_file_desc,sending_buffer,sizeof(sending_buffer))) > 0)
        {
             sending_bytes=send(socket,sending_buffer,file_read_bytes,0);
             
             if(file_read_bytes-sending_bytes!=0)
             {
              perror("file cannot be send properly:Error with Send()");
              close(temp_file_desc);
              return -1;
             }
             
             else{
              sending_file_size=sending_file_size+sending_bytes;
             }

        }
        close(temp_file_desc);
        printf("Transfered %d bytes successfully.\n",sending_file_size);
        printf("Filename %s uploaded successfully.\n",filename);
        return 1;



}



int handle_peer_download(int socket,char filename[50],char filesize[50])
{
      char receiving_buffer[256];
      //char *err="File Doesn't Exist";
      
      int receiving_bytes;
      int receiving_file_size=0;

     int temp_file_desc=open(filename, O_CREAT|O_WRONLY|O_APPEND, 0644);
     if(temp_file_desc<0)
     {
      perror("Error in creating file");
      return -1;
    }
    
    
      while((receiving_bytes= recv(socket,receiving_buffer,256,0)) > 0)
        {    
            int temp_write=write(temp_file_desc,receiving_buffer,receiving_bytes);
            if(temp_write<0)
            {
              perror("Error in writing to file");

            }
            receiving_file_size=receiving_file_size+receiving_bytes;

            
           
            
            //printf("%d\n",receiving_file_size);
            if(atoi(filesize)<=receiving_file_size)
              break;
        }
         

    

        close(temp_file_desc);

        printf("Received %d bytes successfully.\n",receiving_file_size);
        printf("Filename %s Downloaded successfully.\n",filename);

return 1;

}

int createsocket()
{
  int newsocket = socket(AF_INET, SOCK_STREAM, 0);
  return newsocket;
}



int resolve_hostname(char *host_name,char *resolved_ip)
 {  //  printf("%s\n","5" );
      struct in_addr **address_list;
      struct hostent *temp;
      int count=0;

      temp=gethostbyname(host_name);

      if(temp)
      {
        address_list=(struct in_addr **) temp->h_addr_list;
        strcpy(resolved_ip,inet_ntoa(*address_list[count]));
        return 1;
      }
      else
      {
        return -1;
      }
      
}

void append_to_iplist_for_server(char hostname[255],char ipaddress[255],int filedesc,int port)
{
  strcpy(iplist[1].hostname,hostname);
  strcpy(iplist[1].ipaddress,ipaddress);
  //strcpy(iplist[1].filedesc,filedesc);
  iplist[1].listenport=port;
  iplist[1].filedesc=filedesc;
  iplist[1].id=1;
  iplist[1].status=1;
  

}

int handle_connect(char cmd_buf[100],char portnum[6])

{

    char command[50];
    char ip[50];
    char resolved_ip[50];
    char port[10];  
    strcpy(command, strtok(cmd_buf , " "));
    strcpy(ip, strtok(NULL, " "));
    strcpy(port , strtok(NULL, " "));
    //printf("%s\n","2" );
    int flag=0;
    int port1=atoi(port);
    struct sockaddr_in server_addr;
   // memset(&server_addr,0,sizeof(server_addr));

    char host_name[50];
 

    if(resolve_hostname(ip,resolved_ip)<0){
      printf("cannot resolve IP\n");
    }
    if ( inet_pton(AF_INET,resolved_ip, &(server_addr.sin_addr)) < 1 )
        {
          printf("Invalid IP address\n");
          return -1;
        }
        //printf("%s\n",resolved_ip);
        //printf("%d\n",port1);
    
    if(((strcmp(resolved_ip,"127.0.0.1")==0)||(strcmp(resolved_ip,localip)==0)) && ((atoi(portnum))==port1))
    {
      printf("You cannot Connect to your own system\n");
      return -1;
    }

    if((strcmp(resolved_ip,iplist[1].ipaddress)==0) && (port1==iplist[1].listenport))
    {
      printf("You are not allowed to Connect to the server\n");
      return -1;
    }
    
      for(int i=1;i<=9;i++)
    {
        if(server_list[i].status==1)
        {
          if((strcmp(server_list[i].ipaddress,resolved_ip)==0)&&(server_list[i].listenport==port1))
          
                flag=1;
          
          
          }
       

    }
        if(flag==0)
        {
          printf("The peer you are attempting to connect is not registered on the server\n");
          return -1;
        }
        
      
      flag=0;
     

          
      for(int i=1;i<=9;i++)
    {
        if(iplist[i].status==1)
        {
          if((strcmp(iplist[i].ipaddress,resolved_ip)==0) && (iplist[i].listenport==port1))
          
                flag=1;
          
          
          }
       
    }
      
    if(flag)
        {
          printf("Duplicate connection! Connection to the peer already exists !\n");
          return -1;
        }
        
         
        int newsocket=createsocket();
        if(newsocket<0)
        {
          printf("Socket creation failed\n");
          return -1;
        }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port=htons(port1);
    if( connect(newsocket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 ) 
    {
    printf("Unable to Connect to peer : Connection Error\n");
    close(newsocket);
    return -1;
  }
    printf("Successfully connected to peer IP:%s Port: %d \n",resolved_ip,port1 );
 
    iptohostname(resolved_ip,host_name);
    if(send(newsocket,portnum,6,0)<0)
    {
    printf("Sending listen port to the peer failure\n");
    }
    append_to_iplist(host_name,resolved_ip,newsocket,port1);

    return newsocket;
}


int handle_register(char cmd_buf[100],char portnum[6])
{
    

    char command[20];
    char ip[50];
    char resolved_ip[50];
    char port[10];
    char host_name[50];
    char service[20];
    //int port1=atoi(port);
    strcpy(command, strtok(cmd_buf, " "));
    strcpy(ip, strtok(NULL, " "));
    strcpy(port , strtok(NULL, " "));
    int port1=atoi(port);
    struct sockaddr_in sa,server_addr;
    
    server_addr.sin_port=htons(port1);
    //sa.sin_port=htons(port1);
    sa.sin_family=AF_INET;
    server_addr.sin_family = AF_INET;

    if ( inet_pton(AF_INET,ip, &(server_addr.sin_addr)) < 1 )
        {
          printf("Invalid IP address\n");
          return -1;
        }
        
        //server_addr1.sin_addr.s_addr=inet_addr(ip);

    //if((strcmp(ip,"127.0.0.1")==0)||(strcmp(ip,localip)==0))
    //{
      //printf("You cannot Connect to your own system");
      //return -1;
    //}

    if(iplist[1].status==0)

    {
     if( connect(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 ) 
    {
    printf("Unable to Connect to Server : Connection Error\n");
    //close(newsocket);
    return -1;
    }

  }
   else
  {
    printf("Connection to server Exists\n");
    printf("Duplicate connections are not allowed\n");
    return -1;
  }
  
    printf("Successfully connected to IP:%s Port: %d \n",ip,port1 );
    //inet_pton(AF_INET,ip, &sa.sin_addr);
    
    //getnameinfo((struct sockaddr *)&sa, sizeof sa, host, sizeof host, service, sizeof service, NI_NAMEREQD);
        //printf("%s\n", host);
    

    iptohostname(ip,host_name);

    //host1 = gethostbyaddr(&ip41, sizeof(ip41), AF_INET);
    //printf("%s\n",host1->h_name);
    send(server_socket,portnum,6,0);
    append_to_iplist_for_server(host_name,ip,server_socket,port1);



    return 1;    
    }



    

int handle_put(char cmd_buf[100])
{

    int sel_sock;
    char command[10];
    char connect_id[5];
    char filename[50];
    //struct stat stats;
    strcpy(command, strtok(cmd_buf, " "));
    strcpy(connect_id, strtok(NULL, " "));
    strcpy(filename , strtok(NULL, " "));
    char result[50];
    if(atoi(connect_id)==iplist[1].id)
    {
      printf("You cannot request file from Server\n");
      return -1;
    }
    
   if((strcmp(command,"put")==0) && atoi(connect_id) && (sizeof(filename)>0))
    
    {
    int temp_id=0,temp_desc=0;
    for(int i=2;i<=9;i++)
    {
        if(iplist[i].status==1)
        {
          if(iplist[i].id==atoi(connect_id))
          {
              temp_id=iplist[i].id;
              temp_desc=iplist[i].filedesc;
              break;
          
          
          }

     }
   }          sel_sock=temp_desc;
              if((temp_id==0) && (temp_desc==0))
              {
              printf("please enter valid connection id\n");
              return -1;
              }


            if(atoi(connect_id)==1)
             {
              printf("You cannot request file from server\n");
              return -1;
             }
             
             
              int temp_file_desc=open(filename, O_RDONLY);
               

              if( (access(filename,F_OK )<0 || temp_file_desc<0) )
              {
                printf("File not Found\n");
                close(temp_file_desc);
                return -1;
              }
              close(temp_file_desc);

               if(send(sel_sock,"receive",sizeof("receive"),0)<0)
               {
                printf("file request failed ");
                return -1;
              }

                FILE *f;
                char ch;
                int size=0;
                f=fopen(filename,"r");
                fseek(f,0,2);
                size=ftell(f);
                 fclose(f);
                 printf("%d\n",size);


              sprintf(result, "%d",size);


              if(send(sel_sock,filename,sizeof(filename),0)<0){
                printf("File requested to download failed");
                return -1;
             }
              if(send(sel_sock,result,sizeof(result),0)<0){
                printf("File requested to download failed");
                return -1;
             }


             handle_peer_upload(sel_sock,filename);
             return 1;


  }

  else
  {
    printf("Please enter check the format\n"); 
    return -1;
  }


}



int handle_get(char cmd_buf[100])
{
    int sel_sock;
    char message[20];
    memset(message,0,sizeof(message));
    char command[10];
    char connect_id[5];
    char filename[50];
    strcpy(command, strtok(cmd_buf, " "));
    //printf("%s\n",command );
    strcpy(connect_id, strtok(NULL, " "));
    //printf("%s\n",connect_id);
    strcpy(filename , strtok(NULL, " "));
    if(atoi(connect_id)==iplist[1].id)
    {
      printf("You cannot request file from Server\n");
      return -1;
    }
    if((strcmp(command,"get")==0) && atoi(connect_id) && (sizeof(filename)>0))
    
    {
    int temp_id=0,temp_desc=0;
    for(int i=2;i<=9;i++)
    {
        if(iplist[i].status==1)
        {
          if(iplist[i].id==atoi(connect_id))
          {
              temp_id=iplist[i].id;
              temp_desc=iplist[i].filedesc;
              break;
          
          
          }

     }
   }
              sel_sock=temp_desc;
              if((temp_id==0)&& (temp_desc==0))
              {
              printf("please enter valid connection id\n");
              return -1;
              }
              if(atoi(connect_id)==1)
              {
              printf("You cannot request file from server\n");
              return -1;
              }
              if(send(sel_sock,"send",sizeof("send"),0)<0)
               {
                printf("file request failed ");
                return -1;
              }
              if(send(sel_sock,filename,sizeof(filename),0)<0)
               {
                printf("file request failed\n");
                return -1;
              }
              
              if(recv(sel_sock,message,sizeof(message),0)<0)
              {
                printf("Cannot receive ack for filename \n");
                return -1;
              }
             if(strcmp(message,"file_not_found")==0)
             {
                printf("Error: FILE_NOT_FOUND\n");
                return -1;
             }
             if(strcmp(message,"file_found")==0)
             {
              //handle_peer_download(sel_sock,filename);
             }
             
             
    
}
else
{
  printf("Please check commands\n");
  return -1;
}
return 1;

}



int peer_handle_terminate(char cmd_buf[100])
{
    char command[50];
    char connect_id[5];
    int temp_id=0;
    int temp_desc=0;
    char temp_ip[50];
    int temp_port;

    strcpy(command, strtok(cmd_buf, " "));
    strcpy(connect_id, strtok(NULL, " "));
        for(int i=1;i<=9;i++)
    {
        if(iplist[i].status==1)
        {
          if(iplist[i].id==atoi(connect_id))
          {
              temp_id=iplist[i].id;
              temp_desc=iplist[i].filedesc;
              strcpy(temp_ip,iplist[i].ipaddress);
              temp_port=iplist[i].listenport;
              iplist[i].status=0;
              break;
              
          
          }

     }
   }

printf("Connection terminated with peer ID:%d, Peer IP:%s, Port:%d\n",temp_id,temp_ip,temp_port);
 
 return temp_desc;
  
    


}


int initClient(int port)
{
 	int socket2,enable_reuse=1,connect_count=0,fdmax;
    struct sockaddr_in client_addr1,peer_addr;
    char message[MESSAGE_SIZE];char cmd_buf[100];
    char peerIP[32];
    char filename[50];
    int flag_reg=0;
    char portnum[6];
    char host_name[50];
    client_addr1.sin_family=AF_INET;
    client_addr1.sin_addr.s_addr=INADDR_ANY;
    client_addr1.sin_port=htons(port);
    
    reset_status_server();
    
    reset_status_peer();
    

    socket2=socket(AF_INET,SOCK_STREAM,0);
    
    if(socket2<0)
   		{
       		perror("Client:Socket() init failed");
       		return -1;
   		}

      if(bind(socket2,(struct sockaddr *)&client_addr1,sizeof(client_addr1))<0)
      {
            perror("Client:Bind() failed");
            close(socket2);
            return -1;

      }
      if(setsockopt(socket2,SOL_SOCKET,SO_REUSEADDR,&enable_reuse,sizeof(int))<0)
      {
          perror("client:Setsocketopt(SO_REUSEADDR) init failed");
          return -1;
      }

      if ((listen(socket2,MAX_CONN))==0)
      {
            printf("%s\n","Listening" );
      }
      else
      printf("%s\n","Error with listening socket");
      FD_ZERO(&read_fds);
      FD_ZERO(&write_fds);
      FD_ZERO(&client_fds_r);
      FD_ZERO(&client_fds_w);
      FD_SET(STDIN,&client_fds_r);
      FD_SET(socket2,&client_fds_r);
      fdmax=socket2;
      printf("Client is successfully up and running\n");
      printf("type help to know about functionality\n");
      
      server_socket=createsocket();
      if(server_socket<0)
      {
        printf("Server socket cannot be Created\n");
        return -1;
      }

      for(;;)
      {     
             read_fds=client_fds_r;
             write_fds=client_fds_w;
            if (select(fdmax+1, &read_fds, NULL,NULL, NULL)<0)
              {
                  perror("Client:select() failed");
                  continue;
              }
            int sel_sock=0;


            for(sel_sock=0;sel_sock<=fdmax;sel_sock++)
              {   //printf("%s\n","while loop" );

                if(FD_ISSET(sel_sock,&read_fds))
                    {

                        if(sel_sock==socket2)
                        {
                             
                             
                              printf("accepting connections\n");

                              int peer_length = sizeof(peer_addr);
                              int peer_socket=accept(socket2,(struct sockaddr*)&peer_addr,(socklen_t*)&peer_length);
                              if(connect_count==MAX_CONN)
                              {
                              printf("Maximum Connection limit on this server is:%d\n",MAX_CONN);
                              close(peer_socket);
                              }
                              else
                              {
                              if(peer_socket<0)
                              {
                                 perror("Client:accept() failed");
                              }
                             setsockopt(peer_socket,SOL_SOCKET,SO_REUSEADDR,&enable_reuse,sizeof(int));
                             connect_count++;

                             FD_SET(peer_socket,&client_fds_r);

                              printf("Client: New peer has been accepted\n");
                              memset(message,0,MESSAGE_SIZE);
                              int recv_bytes = recv(peer_socket,message,MESSAGE_SIZE,0);


                              if(fdmax<peer_socket)
                                {
                                      fdmax=peer_socket;
                                }
                                if(inet_ntop(AF_INET, &(peer_addr.sin_addr),peerIP,32)<0)
                                {
                                      printf("inet_ntop() failed to convert client IP\n");
                                }
                                //inet_pton(AF_INET, peerIP, &sa.sin_addr);
                                //getnameinfo((struct sockaddr*)&sa, sizeof sa,host_name, sizeof host_name,0, 0, NI_NAMEREQD);
                                //host = gethostbyaddr(&ip4, sizeof(ip4), AF_INET);
                                iptohostname(peerIP,host_name);
                                printf("connection from Peer Hostname:%s,IPaddress:%s,Port:%d\n",host_name,peerIP,atoi(message));
                                append_to_iplist(host_name,peerIP,peer_socket,atoi(message));
                              }
                                
                              }
                              else if(sel_sock==STDIN)

                                {
                                    int byte_count=read(sel_sock , cmd_buf , sizeof(cmd_buf));
                                    if ( byte_count >= 0 )
                                    {
                                        cmd_buf[ byte_count-1] = '\0';
                                    }
              
                                      for(int i = 0;cmd_buf[i] != '\0'; i++)
                                    {
                                      cmd_buf[i] = tolower(cmd_buf[i]);

                                    }
                                    if(strcmp(cmd_buf,"help")==0)
                                    {
                                        get_help();
                                    }
                                       else if(strcmp(cmd_buf,"creator")==0)
                                     {
                                      get_creator();
                                     }
                                      else if(strcmp(cmd_buf,"display")==0)
                                      {
                                      get_ipaddr(port);
                                      }
                                      else if(strcmp(cmd_buf,"list")==0)
                                      {
                                      get_ip_list();
                                      }
                                      else if(strstr(cmd_buf,"connect"))
                                      { 
                                        if(strlen(cmd_buf)>=20)
                                       {
                                        if(connect_count==MAX_CONN)
                                        {
                                          printf("Maximum no of connections reached by peer\n");
                                          
                                        } 
                                        else if(iplist[1].status==0)
                                        {
                                          printf("Please register first with server\n");
                                        }
                                        else{

                                             
                                             snprintf(portnum,6,"%d",port);
                                             int temp=handle_connect(cmd_buf,portnum);
                                             
                                          
                                         if(temp>0)
                                            {

         
                                              fdmax=get_max_scokFD();
                                              FD_SET(temp,&client_fds_r);
                                              connect_count++;


                                            }  
                                            else
                                            {
                                              printf("Problem with connect\n");
                                            }

                                            }
                                            }
                                            else
                                            {
                                              printf("Please check the format of Entered connect Command\n");
                                            }                     
                                      }

                                      else if(strstr(cmd_buf,"register"))
                                      { 
                                        if(strlen(cmd_buf)>=21)
                                        {
                                        snprintf(portnum,6,"%d",port);
                                        int temp=handle_register(cmd_buf,portnum);
                                        if(temp>0)
                                        {
                                          printf("Successfully connected to Server\n");
                                          connect_count++;
                                          FD_SET(server_socket,&client_fds_r);
                                          //fdmax=get_max_scokFD();
                                          if(fdmax<server_socket)
                                            fdmax=server_socket;
                                          //printf("Finished Register\n");

                                          
                                        }
                                        else
                                        {
                                           printf("Problem Connecting to Server\n");

                                        }

                                        }
                                        else
                                          printf("Please check the format of Entered REGISTER Command\n");


                                      }
                                      else if(strstr(cmd_buf,"put"))
                                      { 
                                        if(strlen(cmd_buf)>=7)
                                        {
                                        if(handle_put(cmd_buf)<0)
                                        {
                                          printf("Error occured\n");
                                        }

                                      }
                                      else
                                        printf("Please check the format of Entered PUT Command\n");

                                    }

                                       else if(strcmp(cmd_buf,"quit")==0)
                                      {
                                          
                                          for (int i=1;i<=9;i++)

                                        { 
                                          if(iplist[i].status==1)
                                          {
                                            if(shutdown(iplist[i].filedesc,SHUT_RDWR)<0)
                                            {
                                                printf("Unable to shutdown the socket\n");

                                            }
                                            close(iplist[i].filedesc);
                                            FD_CLR(iplist[i].filedesc,&client_fds_r);
                                            reset_status_peer();
                                            reset_status_server();
                                         
                                        }

                                        }
                                        exit(0);

                                        //fdmax=0;
                                      }
                                       else if(strstr(cmd_buf,"terminate"))
                                      {   if(strlen(cmd_buf)>=11)
                                        {
                                          int peer_dis=peer_handle_terminate(cmd_buf); 
                                          if(peer_dis>0)
                                          {
                                          FD_CLR(peer_dis,&client_fds_r);
                                          close(peer_dis);
                                          fdmax=get_max_scokFD();
                                          reorderids();
                                          printf("%s\n","Peer Successfully terminated");
                                        }
                                        else
                                        {
                                        printf("Problem with handling terminate\n" );
                                        }
                                      }
                                      else
                                        printf("Please check the format of Entered TERMINATE Command\n");
                                      }
                                     else
                                      {
                                      printf("The command you typed is not supported on this Client\n");
                  //continue;
                                      }

                                  }
                                    else if(sel_sock==server_socket)
                                    {  reset_update_list_server();
                                       //printf("1\n");
                                       char temp_count[5];
                                       if(recv(sel_sock, temp_count, sizeof(temp_count), 0)!=0)
                                       {
                                       int number=atoi(temp_count);
                                       handle_server_update_list(number,server_socket);
                                       printf("New peer list registered on server\n");
                                       display_update_server_list();

                                       }
                                       else
                                       {
                                        printf("Client:Server has closed the connection\n");
                                        connect_count--;
                                        iplist[1].status=0;
                                        if(connect_count==0)
                                        {
                                            reset_update_list_client();
                                        }


                                        FD_CLR(sel_sock,&client_fds_r);
                                        fdmax=get_max_scokFD();
                                       }


                                      
                                    }

                                    else
                                    {    char file_name[100];    
                                         memset(file_name,0,sizeof(file_name));
                                         int recvBytes;
                                         char filesize[50];

                                        if ((recvBytes = recv(sel_sock, file_name,sizeof(file_name), 0))!= 0)
                                        {     
                                              if(strcmp(file_name,"send")==0)
                                              {
                                                memset(file_name,0,sizeof(file_name));
                                                recvBytes=recv(sel_sock,file_name,sizeof(file_name), 0);
                                                int temp_file_desc=open(file_name, O_RDONLY);  
                                                //sscanf (message, "%s\n", file_name)
                                                if( (access(file_name,F_OK )<0 || temp_file_desc<0))
                                                {
                                                  printf("File not Found\n");
                                                  close(temp_file_desc);
                                                  send(sel_sock,"file_not_found",sizeof("file_not_found"),0);

                                                } 
                                                else
                                                {
                                                printf("File Found\n");
                                                close(temp_file_desc);
                                                send(sel_sock,"file_found",sizeof("file_found"),0);
                                                //handle_peer_upload(sel_sock,file_name);

                                                //handle_peer_upload(sel_sock,filename);
                                              }


                                              } 
                                              if(strcmp(file_name,"receive")==0)
                                              {
                                                memset(file_name,0,sizeof(file_name));
                                                recvBytes=recv(sel_sock,file_name,sizeof(file_name), 0);
                                                if(recvBytes<0)
                                                {
                                                  printf("%s\n","Receive file name failed");
                                                }
                                                if((recvBytes=recv(sel_sock,filesize,sizeof(filesize), 0))<0)
                                                {
                                                  printf("%s\n","receive file size failed" );
                                                }
                                                
                                                handle_peer_download(sel_sock,file_name,filesize);


                                              } 

                                        }
                                        else
                                        {
                                          printf("Peer disconnected\n");
                                          connect_count--;
                                          for(int i=2;i<=9;i++)
                                          {
                                              if(iplist[i].filedesc==sel_sock)
                                              {
                                                iplist[i].status=0;
                                                printf("Peer disconnected with IP:%s Hostname:%s Port:%d\n",iplist[i].ipaddress,iplist[i].hostname,iplist[i].listenport);
                                                FD_CLR(sel_sock,&client_fds_r);
                                                close(sel_sock);
                                                fdmax=get_max_scokFD();
                                                reorderids();
                                              }

                                          }

                                        }

                                    }


                        }
                        
                         

                        }
                       

                      }



                

            }

