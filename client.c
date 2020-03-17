#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include<sys/wait.h>
#include <unistd.h>
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

extern int errno; 

int port;
int bytes = -1;
int dimensiune;
int nr_ordine = 1;

int AUTENTIFICARE (char * login , char * password , int sockDesc);

int main (int argc , char * argv[])
{
    int sockDesc;
    struct sockaddr_in server;
    char SendAnswer , RecvMess[256] , StartGame[256];
    char login[100];
    char FirstMess[256] = "Unde găseşti un câine fără picioare?\n a) Unde l-ai lasat !! \n b) Acasa!! \n";
    char password[100];

    if (argc != 3)
    { 
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

    port = atoi(argv[2]);

    if ((sockDesc = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

    bzero(&server , sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect (sockDesc, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }   

    bytes =  AUTENTIFICARE(login , password , sockDesc );

    printf ("\nVa rugam sa asteptati , se pot conecta si alti potentiali jucatori !!\n");

    
    if (read (sockDesc , StartGame , 100) < 0)
    {
        perror("\n[client] Eroare la primire semn pt startul jocului : \n");
        return errno;
    }
    printf("%s" , StartGame);
    while (1)
    {
        bzero(RecvMess , 256);
       // bzero(&sockDesc , sizeof(sockDesc));
        if (read (sockDesc , &dimensiune , sizeof(int)) == -1 )
        {
            perror("\n[client] Eroare la primire dimensiune intrebare de la server\n");
            return errno;
        }
        if ( read(sockDesc , RecvMess , dimensiune) <= 0)
        {
            perror("\n[client] Eroare la primire intrebareee de la  server\n");
            return errno;
        }
        if (nr_ordine == 1) { strcpy(RecvMess , FirstMess); nr_ordine++;}
        if (strstr(RecvMess , (char *) "Castigatorul Quizzzzzz-ului este jucatorul cu numarul"))
        {
            printf("\n%s\n" , RecvMess);
            break;
        }
        else 
            if (strcmp(RecvMess, (char *) "Felicitari a-ti Castigat , deoarece ultimul adversar a parasit jocul !") == 0)
            {
                printf("\n%s\n" , RecvMess);
                break;
            }
        int flags = fcntl(0, F_GETFL, 0);
        fcntl(0, F_SETFL, flags | O_NONBLOCK);
        bytes = -1;
        int contor = 30;
        while (bytes < 0 && contor >= 0)
        {
             printf("Au ramas %d secunde \n" , contor);
             sleep(1);
             printf("\033[H\033[J");
             printf("\nPENTRU A IESI DIN JOC TASTATI BUTONUL Q : \nAveti 30 de secunde pentru a raspunde :\nINTREBAREA :  %s\n" , RecvMess);
             bytes = read (0 , &SendAnswer, 100);
             contor--;
        }
        fflush(stdout);
        if ( (write(sockDesc , &SendAnswer , sizeof(char)) < 0))
        {
            perror("\n[client] Eroare la primire Triimitere Intrebare catre server\n");
            return errno;
        }
        if (SendAnswer == 'Q') 
            break;
    }
    close(sockDesc);
    sleep(2);
    return 0;

}

int AUTENTIFICARE (char * login , char * password , int sockDesc)
{   
    int valoareAdevar = 0;
    while(!valoareAdevar)
    {
            printf("\nIntroduce-ti datele pt a v-a autentifica :\nLogin : ");
            bytes = -1;
            while (bytes <= 0)
            {
               fflush(stdout);
               bytes = scanf("%s" , login); // bytes =  scanf("%s" , login);  
            }
            printf("\nParola: ");
            bytes = -1;
            while (bytes <= 0)
            {
                fflush(stdout);
                bytes = scanf("%s" , password);  // read (0 , password , 100);   
            }

            int fdW , fdR;
            if ( (fdW = write(sockDesc , login , 100)) <= 0)
            {
                perror("\n[client] Eroare la transmitere date autentificare catre server1\n");
                return errno;
            }
            if ( (fdR =  write(sockDesc , password , 100)) <= 0)
            {
                perror("\n[client] Eroare la transmitere date autentificare catre server2\n");
                return errno;
            }
            bytes = read(sockDesc , &valoareAdevar , sizeof(int) );
    }
     
    
    return bytes;
}