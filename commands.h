//
//  commands.h
//  
//
//  Created by Madhav on 10/2/16.
//
//

#ifndef commands_h
#define commands_h



typedef struct{
    char hostname[255];
    char ipaddress[255];
    int filedesc;
    int port;
    int listenport;
    int status;
    int id;
} IPLIST;

typedef struct {
    char hostname[255];
    char ipaddress[255];
    int port;
    
} CLIENT_TRANSMIT;

#endif /* commands_h */
