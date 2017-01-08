//  hemamadh_proj1.c
//  
//
//  Created by Hema Madhav Jakkampudi on 9/24/16.
//

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
#include "server.c"
#include "client.c"
//#include <linux/if_link.h>




void programUsage(int argc,char *argv[])
{
    
    printf("\tUsage: %s <APPLICATION MODE> <LISTENING_PORT>\n",argv[0]);
    printf("\tAcceptable values for <APPLICATION MODE> are 's' or 'c' ; Server and Client modes respectively.\n");
    printf("\tPlease make sure that <LISTENING_PORT_VALUE> more than 1024.Ports less than this are reserved for System Usage");
    printf("---------------------------------------------------------------------------------\n");
}

int main(int argc,char *argv[])
{
    //char PROGRAM_MODE=argv[1];
    int PORT=atoi(argv[2]);
    
        if(argc !=3)
       {
        
            printf("\tPlease check the number of command line arguments \n");
            programUsage(argc,argv);
            return -1;
            
        }
    
    
    if (PORT>1024) 

    {
        
        
        if(strcmp(argv[1],"c")==0)
          {
            initClient(PORT);
            printf("Client\n");
          }

         else if(strcmp(argv[1],"s")==0)
          {
            //initClient(PORT);
            initServer(PORT); 
            printf("Server\n");
          }
          else 
          {
                printf("--------------------------------------------------------------------------------\n");
                printf("\tERROR: Invalid arguments for the mode Server or Client \n");
                programUsage(argc,argv);
                return -1 ;
        }
    }

    else
    {
        
   printf("Port value should be morethan than 1024\n");

    }
    
    }





    
    

