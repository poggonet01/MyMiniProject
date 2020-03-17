#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include<sys/signal.h>
#include <unistd.h>
#include <errno.h>
#include<sys/select.h>
#include<sqlite3.h>
#include<fcntl.h>
#include <stdio.h>
#include<sys/wait.h>
#include <sys/mman.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include<time.h>
#include<ctype.h>

#define PORT 2128
#define tLstCl 10
#define NMAX 512

extern int errno;
int fd , nfds , freq[1001];

char RecQuest;
int ok = 1, *jucatori , CopieJucatori = 0 , *NrProcess ,  cont = 0;
int CopieJucatori2 = 0;
fd_set writefds;
fd_set readfds;
fd_set actfds;
struct timeval tv;		
int sockDesc, client;		/* descriptori de socket */
int optval=1; 			/* optiune folosita pentru setsockopt()*/ 
int length;
struct sockaddr_in server;
struct sockaddr_in from;
int valAd = 0;
int *timp;
int dimensiune;
int forward = 0;
char SendQuest[256];
char * SendAnswer;
int id_num = 0;
int nr_intreb = 0;
sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;
int OrdineJucator = 0;
int fd_buff;
char FILEusername[NMAX] = "CerereLog.txt";
struct Scor 
{
  int juc1;
  int juc2;
  int juc3;

}EvalScor;

/* functie de convertire a adresei IP a clientului in sir de caractere  (luata de pe site-ul cursului ) */
char * conv_addr (struct sockaddr_in address)
{
  static char str[25];
  char port[7];

  /* adresa IP a clientului */
  strcpy (str, inet_ntoa (address.sin_addr));	
  /* portul utilizat de client */
  bzero (port, 7);
  sprintf (port, ":%d", ntohs (address.sin_port));	
  strcat (str, port);
  return (str);
}

/*functie de kill pt procesele zombi */
void My_HANDLER(int cod)
{
  int saved_errno = errno;
  while ( (waitpid((pid_t)(-1) , 0 , WNOHANG)) > 0 ) {}
  errno = saved_errno;
}

void SQL_CREATE_INSERT (char * Create)
{
  char *errmsg;
  int   ret;

  ret = sqlite3_exec(db, Create, 0, 0, &errmsg);

  if(ret != SQLITE_OK) {
    printf("Eroare la creare Tabel: %s [%s].\n", Create, errmsg);
  }
}
int CheckAnswer(int id_num , char RecQuest);
int PREPARE_QUIZ(int id_num , char * ThrowBack , char * sql);
int SendQuestion(int fd);
int Logare (int fd );
int Quiz (int fd);
void TryToSel();
int CreateProcess (int *jucatori , int *NrProcess , int *timp);
int LogareUltimJuc (int fd );
void ScoateJucator(int fd);

int main ()
{
    EvalScor.juc1 = 0;
    EvalScor.juc2 = 0;
    EvalScor.juc3 = 0;
    if ((sockDesc = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server] Eroare la socket().\n");
      return errno;
    }

    setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

    if (bind (sockDesc, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server] Eroare la bind().\n");
      return errno;
    }

    if (listen (sockDesc, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    SQL_CREATE_INSERT("DROP TABLE IF EXISTS QUESTIONS;" \
      "CREATE TABLE QUESTIONS("  \
      "ID_QUEST INT PRIMARY KEY     NOT NULL," \
      "QUEST           TEXT    NOT NULL );");

    SQL_CREATE_INSERT ("DROP TABLE IF EXISTS ANSWER;" \
      "CREATE TABLE ANSWER("  \
      "ID_ANSWER INT PRIMARY KEY     NOT NULL," \
      "NR_ORD    CHAR(2)    NOT NULL," \
      "ANS          TEXT    NOT NULL," \
      "VAL_AD           CHAR(2)     NOT NULL," \
      "ID_QUEST       INT     NOT NULL );");
    
    SQL_CREATE_INSERT ("INSERT INTO QUESTIONS (ID_QUEST,QUEST) "  \
         "VALUES (1, 'Unde găseşti un câine fără picioare?' ); " \
         "INSERT INTO QUESTIONS (ID_QUEST,QUEST) "  \
         "VALUES (2, 'Cum iese un cal alb, care intră în Marea Neagră?'); " \
          "INSERT INTO QUESTIONS (ID_QUEST,QUEST) "  \
         "VALUES (3, 'Câte mere poți mânca dimineața pe stomacul gol? '); " \
          "INSERT INTO QUESTIONS (ID_QUEST,QUEST) "  \
         "VALUES (4, 'Într-un coș sunt 3 mere verzi și 4 roșii. Câte mere sunt în coș?'); " \
         "INSERT INTO QUESTIONS (ID_QUEST,QUEST) "  \
         "VALUES (5, 'Dacă 3 ouă fierb în 10 minute, 56 de ouă în câte minute fierb?');");

   SQL_CREATE_INSERT("INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (1, 'A' , ' a) Unde l-ai lasat !! ' , 'A' , 1 ); " \
       "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (2, 'B' , ' b) Acasa !! ' , 'F' , 1 ); " \
       "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (3, 'A' , ' a) Negru!! ' , 'F' , 2 ); " \
      "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (4, 'B' , ' b) Ud!  ' , 'A' , 2 ); " \
      "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (5 , 'A' , ' a) Unul, căci restul nu mai sunt pe stomacul gol !! ' , 'A' , 3 ); " \
      "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (6, 'B' , ' b) Cate Vreai !! ' , 'F' , 3 ); " \
      "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (7, 'A' , ' a) Sunt 7 mere !! ' , 'F' , 4 ); " \
      "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (8, 'B', ' b) Trei, restul sunt 4 legume!! ' , 'A' , 4 ); " \
      "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (9, 'A' , ' a) In foarte mult timp !! ' , 'F' , 5 ); " \
      "INSERT INTO ANSWER (ID_ANSWER,NR_ORD,ANS,VAL_AD,ID_QUEST) "  \
         "VALUES (10, 'B' , ' b) In 10 minute !! ' , 'A' , 5 ); ");
    

    FD_ZERO(&actfds);
    FD_SET(sockDesc , &actfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    nfds = sockDesc;

    timp = mmap(NULL, sizeof *timp, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    jucatori = mmap(NULL, sizeof *jucatori, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    NrProcess = mmap(NULL, sizeof *NrProcess, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *NrProcess = 0;    *jucatori = 0; *timp = 1;
    printf ("[server] Asteptam la portul %d...\n", PORT);
    fflush (stdout);
    while(1)
    {/*  */
      pid_t pid;
     if (CreateProcess(jucatori , NrProcess,timp))
     {
       *jucatori = 0; cont = 0;
        if (-1 == (pid = fork()))
        {
            perror("[server] Eroare la fork () ");
            return errno;
        }
        else 
        if (!pid)
        {
              //printf("\nNrProcess COPIL = %d \n" , *NrProcess);
              while (1)
              {
                if (*jucatori <= 3)
                {
                    TryToSel();
                    int fd = 0;
                    while (fd <= nfds)
                    {
                      if ((freq[fd] != 0 && *jucatori >= 2 ) || (freq[fd] != 0 &&  *NrProcess > 1 && CopieJucatori >= 2) )
                        {
                          if (Quiz(fd)) 
                          {
                              printf("\nAm adresat intreabarea : %d \n" , cont );
                              cont++;
                          }
                        }
                        else
                        if (fd != sockDesc && FD_ISSET(fd , &readfds))
                        {	
                            if (SendQuestion(fd))
                            fflush (stdout); 
                        }
                      if (ok == 1 && *jucatori == 3) {fd = 0 ; ok = 0;}
                      else                            fd++;
                    }
                }
              }
        }
        else 
        {
          *NrProcess = *NrProcess + 1;
          *timp = 1;
         // printf("\nNrProcesssssssssssssss = %d \n" , *NrProcess);
          signal(SIGCHLD , My_HANDLER);
        }
      }
    }
}
int CreateProcess (int *jucatori , int *NrProcess , int *timp)
{
  if ((*timp >= 9 && *jucatori == 2 ))
    return 1;
  if (*NrProcess == 0 && *jucatori == 0)
    return 1;
  if ( (*NrProcess != 0 ) && (*jucatori == 3) )
      return 1;
  if ( (*NrProcess != 0 ) && (*jucatori <= 3) )
    return 0;
  return 0;
}
void TryToSel()
{
  bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));
  if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0)
     {
          perror ("[server] Eroare la select().\n");
          return errno;
     }
                 if (FD_ISSET (sockDesc, &readfds))
                {
               
                    length = sizeof (from);
                    bzero (&from, sizeof (from));

                    /* a venit un client, acceptam conexiunea */
                    client = accept (sockDesc, (struct sockaddr *) &from, &length);

                    /* eroare la acceptarea conexiunii de la un client */
                    if (client < 0)
                    {
                        perror ("[server] Eroare la accept().\n");
                      //  continue;
                    }

                    if (nfds < client) /* ajusteaza valoarea maximului */
                        nfds = client;
                            
                    /* includem in lista de descriptori activi si acest socket */
                    FD_SET (client, &actfds);

                    printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n",client, conv_addr (from));
                    fflush(stdout);
                }
}

int SendQuestion(int fd)
{
    int fdR , fdW , bytes;
   // char login[100] , password[100];
    if (freq[fd] == 0)    bytes = Logare(fd);
    else                bytes =  Quiz(fd);
    return bytes;
}

int Quiz ( int fd)
{
    int bytes;
    if (CopieJucatori2 == CopieJucatori)
    {
          if ( (CopieJucatori > 2 ) && ( cont % 3  == 0 ))   id_num = id_num + 1;
          else   if ( (CopieJucatori < 3)  && (cont  % 2 == 0) )  id_num = id_num + 1; 

          if (CopieJucatori > 2)  {
            if (OrdineJucator >= 3) OrdineJucator = 1;
            else                     OrdineJucator +=1;
          }
          else if (CopieJucatori < 3)
          {
            if (OrdineJucator >= 2) OrdineJucator = 1;
            else                     OrdineJucator +=1;
          }
    }
    else if (CopieJucatori2 != CopieJucatori)
    {
      if (EvalScor.juc1 == -1)
      {
        if (OrdineJucator >= 3) { OrdineJucator = 2; id_num = id_num + 1; }
        else                    OrdineJucator +=1;
      }
      else if (EvalScor.juc2 == -1)
      {
         if (OrdineJucator >= 3) {OrdineJucator = 1; id_num = id_num + 1;}
        else                    OrdineJucator +=1;

        if (OrdineJucator == 2) OrdineJucator = 3;
      }
      else if (EvalScor.juc3 == -1)
      {
          if (OrdineJucator >= 2) {OrdineJucator = 1; id_num = id_num + 1;}
          else                    OrdineJucator +=1;
      }
      
    }
    if (id_num >= 6)
    {        
      int Max;
      if ( (EvalScor.juc1 > EvalScor.juc2) && (EvalScor.juc1 > EvalScor.juc3) )
          Max = 1;
      else 
          if ( (EvalScor.juc2 > EvalScor.juc1) && (EvalScor.juc2 > EvalScor.juc3) )
              Max = 2;
          else 
               if ( (EvalScor.juc3 > EvalScor.juc2) && (EvalScor.juc3 > EvalScor.juc1) )
                      Max = 3;
      sprintf (SendQuest , "Castigatorul Quizzzzzz-ului este jucatorul cu numarul %d " , Max);
      dimensiune = strlen(SendQuest);
      if (write(fd , &dimensiune , sizeof(int)) <= 0)
          {
              perror("\n[Server] Eroare la transmitere dimensiune intrebare\n");
              return errno;
          }
      if (write(fd , SendQuest , dimensiune ) <= 0)
          {
              perror("\n[Server] Eroare la transmitere dimensiune intrebare\n");
              return errno;
          }
      ScoateJucator(fd);
    }
    else 
    {
         sql = "SELECT QUEST from QUESTIONS where ID_QUEST=?";
         bzero(SendQuest , 256);
        // strcat(SendQuest , (char *)"Pentru a iesi din joc tastati litera Q : \n");
        int val = PREPARE_QUIZ( id_num , SendQuest,sql);

         strcat(SendQuest , (char *)"\n");

        //printf ("\nAici inca sunt %s \n" , SendQuest);

          sqlite3_stmt *res = 0;

          sql = "SELECT ANS from ANSWER where ID_QUEST=?";

      
          rc = sqlite3_prepare_v2(db, sql, -1, &res, NULL);
          if (rc != SQLITE_OK) {
              printf("Failed to prepare statement: %s\n\r", sqlite3_errstr(rc));
              sqlite3_close(db);
              return 1;
          } 

          if (id_num != 0)   rc = sqlite3_bind_int(res, 1, id_num);
          else                rc = sqlite3_bind_int(res, 1, 1);

          if (rc != SQLITE_OK) {
                printf("Failed to prepare statement: %s\n\r", sqlite3_errstr(rc));
                sqlite3_close(db);
                return 1;
          } 

          while ( (rc = sqlite3_step(res)) == SQLITE_ROW)
          {
            strcat(SendQuest , sqlite3_column_text(res, 0));
            strcat(SendQuest , (char *) "\n");
          }
         
           dimensiune = strlen(SendQuest);
           
          if (write(fd , &dimensiune , sizeof(int)) <= 0)
          {
              perror("\n[Server] Eroare la transmitere dimensiune intrebare\n");
              return errno;
          }
          if ( write(fd , SendQuest , dimensiune) <= 0 )
          {
              perror("\n[Server] Eroare la transmitere intrebare\n");
              return errno;
          }
          bzero(SendQuest , 256);
          bytes = -1;
          while (bytes < 0){
              bytes = read (fd , &RecQuest , sizeof(char));
          }
          printf ("[server]Mesajul a fost receptionat..//.%c\n", RecQuest);

        if (val = CheckAnswer(id_num  ,  RecQuest) == 1 ) 
        {
          switch (OrdineJucator)
          {
            case 1 : { EvalScor.juc1 += 1; printf ("\nScorul acumulat de catre primul jucator este : %d \n" , EvalScor.juc1); }; break;
            case 2 : { EvalScor.juc2 += 1; printf ("\nScorul acumulat de catre al doilea jucator este : %d \n" , EvalScor.juc2); }; break;
            case 3 : { EvalScor.juc3 += 1; printf ("\nScorul acumulat de catre al treilea jucator este : %d \n" , EvalScor.juc3); }; break;

          }
        }
        else if (val = CheckAnswer(id_num  ,  RecQuest) == 0)
        {
          switch (OrdineJucator)
          {
            case 1 : { printf ("\nScorul acumulat de catre primul jucator este : %d \n" , EvalScor.juc1); }; break;
            case 2 : { printf ("\nScorul acumulat de catre al doilea jucator este : %d \n" , EvalScor.juc2); }; break;
            case 3 : { printf ("\nScorul acumulat de catre al treilea jucator este : %d \n" , EvalScor.juc3); }; break;
          }
        }
        else 
          {
            ScoateJucator(fd);
             if (CopieJucatori2 < 2)
            {
              for (int i = 1; i <= nfds; i++)
              {
                if (freq[i] == 1)
                {
                   strcpy (SendQuest, (char *) "Felicitari a-ti Castigat , deoarece ultimul adversar a parasit jocul !");
                   dimensiune = strlen(SendQuest);
                   if (write(i , &dimensiune , sizeof(int)) <= 0)
                    {
                          perror("\n[Server] Eroare la transmitere dimensiune\n");
                          return errno;
                    }
                    if ( write(i , SendQuest , dimensiune) <= 0 )
                    {
                        perror("\n[Server] Eroare la transmitere intrebare\n");
                        return errno;
                    }
                  ScoateJucator(i);
                }
              }
            }
          }

    return bytes;
    }
  
}

void ScoateJucator(int fd)
{
   
   printf ("[server] S-a deconectat clientul cu descriptorul %d.\n",fd);
   fflush (stdout);
   close (fd); FD_CLR (fd, &actfds);/* scoatem si din multime */   
   freq[fd] = 0;
   CopieJucatori2--;
 //  CopieJucatori--;

    if (CopieJucatori2 >= 2)
   {
      switch(OrdineJucator)
      {
        case 1 : EvalScor.juc1 = -1; break;
        case 2 : EvalScor.juc2 = -1; break;
        case 3 : EvalScor.juc3 = -1; break;
      }
    
   } 
}

int CheckAnswer(int id_num , char  RecQuest)
{ 
   if (RecQuest == 'Q')
      return -1;
   if (RecQuest != 'A' && RecQuest != 'B')
      return 0;
   sqlite3_stmt *res = 0;
   sql = "SELECT VAL_AD FROM ANSWER WHERE ID_QUEST=? AND NR_ORD=? ";

   rc = sqlite3_prepare_v2(db, sql, -1, &res, NULL);
   if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n\r", sqlite3_errstr(rc));
        sqlite3_close(db);
        return 1;
   }

    if (id_num != 0)   rc = sqlite3_bind_int(res, 1, id_num);
    else                rc = sqlite3_bind_int(res, 1, 1);

    sqlite3_bind_text(res , 2 , &RecQuest,sizeof(char), SQLITE_STATIC);

    rc = sqlite3_step(res);

    printf("\n CheckAnswer  %s \n" ,sqlite3_column_text(res, 0) );

    if (strcmp(sqlite3_column_text(res, 0) , (char *)"A") == 0)
        return 1;
    return 0;
}


int PREPARE_QUIZ(int id_num , char * ThrowBack , char * sql)
{
   sqlite3_stmt *res = 0;
  rc = sqlite3_prepare_v2(db, sql, -1, &res, NULL);
     if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n\r", sqlite3_errstr(rc));
        sqlite3_close(db);
        return 1;
    } 
    if (id_num > 0)   rc = sqlite3_bind_int(res, 1, id_num);
    else                rc = sqlite3_bind_int(res, 1, 1);

    if (rc != SQLITE_OK) {
          printf("Failed to prepare statement: %s\n\r", sqlite3_errstr(rc));
          sqlite3_close(db);
          return 1;
    } 
    rc = sqlite3_step(res);
    strcat(ThrowBack , sqlite3_column_text(res, 0) );
   
    sqlite3_finalize(res);
    return 1;

}

int Logare (int fd)
{
  
   int fdR , fdW;
   time_t initialtime = clock();
   char password[100] , login[100];
   int x = 9;
   char bufferFromFile[1024];
   valAd = 0;
    if (-1 == (fd_buff = open(FILEusername , O_RDWR)))
        {
            perror ("\nEroare la deschidere fisier(USERNAME) \n");
            return errno;
        }
      if (-1 == (read(fd_buff , bufferFromFile , 1024 )))
        {
                perror("\nEroare de citire din fisierul cu username\n");
                return errno;
        }
   while (1)
   {
        //bzero(bufferFromFile , 0);
      printf("\n[server] Asteptam datele de autentificare : \n");
      if (-1 == (fdR = read (fd , login , 100 )))
      {
          perror("\n[Server] Eroare la citire login\n");
          return errno;
      }
      printf ("[server]Mesajul a fost receptionat...%s\n", login);
      if (-1 == (fdR = read (fd , password , 100)))
      {
          perror("\n[Server] Eroalre la citire parola\n");
          return errno;
      }
      printf ("[server]Mesajul a fost receptionat...%s\n", password);

      if (strstr(bufferFromFile, login))
      {
          if (strstr(bufferFromFile , password))
          {
               int send = 1;
              fdW =  write(fd , &send , sizeof(int));
              break;
          }
      }
      else 
      {
        int send = 0;
        fdW = write(fd , &send , sizeof(int));
      }
   }
   close(fd_buff);
    
     *jucatori = *jucatori + 1;
    CopieJucatori = CopieJucatori + 1;
    CopieJucatori2 = CopieJucatori2 + 1;
    printf ("[server]Numarul de jucatori in sistem sunt %d\n", *jucatori);
    freq[fd] = 1;
    int DublNfds = nfds;
    int fd1 = sockDesc;
    if (*jucatori >=  2)
    {
      initialtime = clock();
      while (*jucatori < 3)    
      {
          if ((clock() - initialtime) / CLOCKS_PER_SEC >= x) 
              break;
          if ((clock() - initialtime) / CLOCKS_PER_SEC >= *timp )
          {
              printf("Au trecut %d secunde \n" , *timp );
              TryToSel();
              
              if (DublNfds != nfds)
              {
                  for (; fd1 != nfds; fd1++)
                  {
                       if (FD_ISSET ( fd1 , &readfds )) 
                      {
                          if (LogareUltimJuc(nfds ))
                          //printf ("[server] S-a deconectat clientul cu descriptorul %d.\n",fd);
                          fflush (stdout);
                          //close (fd); //FD_CLR (fd, &actfds); scoatem si din multime       
                      }   
                  }
                  break;
              }
              *timp = *timp + 1;
          } 
      }
      *timp = 1;
       for (int i = 4; i <= nfds; i++)
      {
          if (freq[i])
          {
             write(i, "\nSA INCEAPA JOCUL !!\n" , 22);
          }
               
      }
    }
    return fdR;
}
int LogareUltimJuc (int fd)
{
  int fdR , fdW;
  char bufferFromFile[1024];
  char password[100] , login[100];
  if (-1 == (fd_buff = open(FILEusername , O_RDWR)))
    {
        perror ("\nEroare la deschidere fisier(USERNAME) \n");
        return errno;
    }
   if (-1 == (read(fd_buff , bufferFromFile , sizeof(bufferFromFile) )))
      {
              perror("\nEroare de citire din fisierul cu username\n");
              return errno;
      }
  while (1)
  {
      printf("\n[server] Asteptam datele de autentificare ::: \n");
      if (-1 == (fdR = read (fd , login , 100 )))
      {
          perror("\n[Server] Eroare la citire logiiiiiiiiiiiiiiiiin\n");
          return errno;
      }
      printf ("[server]Mesajul a fost receptionat....................%s\n", login);
      if (-1 == (fdR = read (fd , password , 100)))
      {
          perror("\n[Server] Eroalre la citire parola\n");
          return errno;
      }
      printf ("[server]Mesajul a fost receptionat...%s\n", password);

      if (strstr(bufferFromFile,login) && strstr(bufferFromFile , password))
      {
          int send = 1;
        fdW =  write(fd , &send , sizeof(int));
        break;
      }
      else 
      {
        int send = 0;
        fdW = write(fd , &send , sizeof(int));
      }
  }
  close(fd_buff);
    *jucatori = *jucatori + 1;
    CopieJucatori = CopieJucatori + 1;
    CopieJucatori2 = CopieJucatori2 + 1;
     printf ("[server]Numarul de jucatori in sistem sunt %d\n", *jucatori);
    freq[fd] = 1;
    return fdR;
}