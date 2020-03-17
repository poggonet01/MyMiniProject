#include"pch.h"
#include<iostream>
#include<windows.h>
#include<MMSystem.h>
#include<stdio.h>
#include<graphics.h>
#include<cstring>
#include<conio.h>

using namespace std;

int ok = 1, continua = 0;
int ordineJucator = 1;
int numar, width, height,  latura,  sus, stanga,urm = 1;
int nr_de_ordine = 1, DimNava;
int Player1_harta[11][11], Player2_harta[11][11];
int Player1_shoot[11][11], Player2_shoot[11][11];
int ordine, contor, contor1;

DWORD ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
DWORD ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

struct Nave_Jactori
{
    int nrDeLov1;
    int nrDeLov2;
    int navaDePatru1, navaDePatru2;
    int navaDeTrei1, navaDeTrei2 ;
    int naveleDeDoi1, naveleDeDoi2;
    int toateNavele1, toateNavele2;
} stare;

void init_structura ()
{
    stare.nrDeLov1 = stare.nrDeLov2 = 7;
    stare.navaDePatru1 = stare.navaDePatru2 = 4;
    stare.navaDeTrei1 = stare.navaDeTrei2 = 3;
    stare.naveleDeDoi1 = stare.naveleDeDoi2 = 4;
    stare.toateNavele1 = stare.toateNavele2 = 11;
}
void init_Harta ()
{
    int gd = DETECT, gm;
    initgraph(&gd, &gm,(char *) "C:\\TC\\BGI");

    initwindow(ScreenWidth, ScreenHeight, "", -3, -3);
}
void put_in_matrix()
{
    int linia,coloana,x,y;
    int x1, y1, x2, y2;
    int xmijloc, ymijloc;
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        x=mousex();
        y=mousey();

        linia=(y-sus)/latura+1;
        coloana=(x-stanga)/latura+1;
        if (urm == 1)      Player1_harta[linia][coloana]=1;
        else               Player2_harta[linia][coloana] = 1;
        x1=stanga+latura*(coloana-1);
        y1=sus+latura*(linia-1);
        x2=x1+latura;
        y2=y1+latura;
        xmijloc=(x1+x2)/2;
        ymijloc=(y1+y2)/2;
        setfillstyle(SOLID_FILL,LIGHTGRAY);
        //Conditia pt a nu desena inafara hartii
        if (x >= stanga && y >= sus && (x < stanga + (10 * latura )) && (y < sus + (10 * latura)) )
        {
            // int Point[]={xmijloc-latura/2 +5,ymijloc,xmijloc + latura /2,ymijloc,xmijloc+latura/4,ymijloc+latura/2+2,xmijloc-latura/4+5,ymijloc+latura/2+2,xmijloc-latura/2+5,ymijloc};
            bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
            //drawpoly(5 , Point);
            // fillpoly(5 , Point);
        }
    }
}

void EvidentaStariiAdversarului (int DimNava, int ordine)
{
    if (DimNava == 4)
    {
        if (!ordine)
        {
            stare.navaDePatru2--;
            stare.toateNavele2--;
            if (stare.navaDePatru2 <= 0)
                stare.nrDeLov2 = stare.nrDeLov2 - 3;

        }
        else
        {
            stare.toateNavele1--;
            stare.navaDePatru1--;
            if (stare.navaDePatru1 <= 0)
                stare.nrDeLov1 = stare.nrDeLov1 - 3;
        }


    }
    else if (DimNava == 3)
    {
        if (!ordine)
        {
            stare.toateNavele2--;
            stare.navaDeTrei2--;
            if (stare.navaDeTrei2 <= 0)
                stare.nrDeLov2 = stare.nrDeLov2 - 2;
        }
        else
        {
            stare.navaDeTrei1--;
            stare.toateNavele1--;
            if (stare.navaDeTrei1 <= 0)
                stare.nrDeLov1 = stare.nrDeLov1 - 2;
        }
    }
    else
    {
        if (!ordine)
        {
            stare.naveleDeDoi2--;
            stare.toateNavele2--;
            if (stare.naveleDeDoi2 <= 2 && stare.naveleDeDoi2 != 1)
                stare.nrDeLov2 = stare.nrDeLov2 - 1;

        }
        else
        {
            stare.naveleDeDoi1--;
            stare.toateNavele1--;
            if (stare.naveleDeDoi1 <= 2 && stare.naveleDeDoi1 != 1)
                stare.nrDeLov1 = stare.nrDeLov1 - 1;
        }
    }
    if (stare.toateNavele1 == 0)
    {
        outtextxy(50, 300, (char *) "A castigat al doilea");
        continua = ok = 0;
    }

    else if (stare.toateNavele2 == 0)
    {
        outtextxy(50, 300, (char *) "A castigat primul");
        continua = ok = 0;
    }

}
int AfluDimNavei(int Matrice[11][11], int line, int col)    // Moementan nu folosesc nicaieri functia asta
{
    int Dimensiune = 1;
    int masor_inversLine;
    int masor_inversCol;
    if (Matrice[line-1][col])
    {
        masor_inversLine = line+1;
        line--;
        while (Matrice[line][col])
        {
            line--;
            Dimensiune++;
        }
        while (Matrice[masor_inversLine][col] == 1)
        {
            Dimensiune++;
            masor_inversLine++;
        }

    }
    else if (Matrice[line+1][col])
    {
        masor_inversLine = line - 1;
        line++;
        while (Matrice[line][col])
            line++, Dimensiune++;
        while (Matrice[masor_inversLine][col] == 1)
        {
            masor_inversLine--;
            Dimensiune++;
        }
    }
    else if (Matrice[line][col-1])
    {
        masor_inversCol = col + 1;
        col--;
        while (Matrice[line][col])
            col--, Dimensiune++;
        while (Matrice[line][masor_inversCol] == 1)
        {
            masor_inversCol++;
            Dimensiune++;
        }
    }
    else if (Matrice[line][col+1])
    {
        col++;
        while (Matrice[line][col])
            col++, Dimensiune++;
    }
    else if (Matrice[line-1][col-1])
    {
        masor_inversCol = col+1;
        masor_inversLine = line + 1;
        line--, col--;
        while (Matrice[line][col])
            col--, line--, Dimensiune++;
        while (Matrice[masor_inversLine][masor_inversCol] == 1)
            masor_inversCol++, masor_inversLine++, Dimensiune++;
    }
    else if (Matrice[line+1][col+1])
    {
        masor_inversCol = col - 1;
        masor_inversLine = line - 1;
        line++, col++;
        while (Matrice[line][col])
            line++, col++, Dimensiune++;
        while (Matrice[masor_inversLine][masor_inversCol] == 1)
            masor_inversCol--, masor_inversLine--, Dimensiune++;
    }
    else if (Matrice[line-1][col+1])
    {
        masor_inversCol = col - 1;
        masor_inversLine = line + 1;
        line--, col++;
        while (Matrice[line][col])
            line--, col++, Dimensiune++;
        while (Matrice[masor_inversLine][masor_inversCol] == 1)
            masor_inversCol--, masor_inversLine++, Dimensiune++;
    }
    else if (Matrice[line+1][col-1])
    {
        masor_inversCol = col + 1;
        masor_inversLine = line - 1;
        line++, col--;
        while (Matrice[line][col])
            line++, col--, Dimensiune++;
        while (Matrice[masor_inversLine][masor_inversCol] == 1)
            masor_inversCol++, masor_inversLine--, Dimensiune++;
    }
    return Dimensiune;
}
void Restrictii (int NrShoot)
{
    char arr[20];
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    sprintf(arr, "Ai dreptul la %d lovituri", NrShoot);
    outtextxy(20, 200, arr);

}
void MarcheazaLovituriPrim () // Functia in care marchez loviturile primului jucator
{
    int linia,coloana,x,y;
    int x1, y1, x2, y2;
    int xmijloc, ymijloc;
    // Reinitializez de fiecare data matricea cu evidenta a loviturilor jucatorului
    for (int i =1 ; i <= 10; i++)
    {
        for (int j = 1; j <= 10; j++)
        {
            x1=stanga+latura*(j-1);
            y1=sus+latura*(i-1);
            x2=x1+latura;
            y2=y1+latura;
            xmijloc=(x1+x2)/2;
            ymijloc=(y1+y2)/2;

            if (Player1_shoot[i][j])
            {
                if (Player2_harta[i][j])
                {
                    //Daca am dat in nava adversarului

                    setfillstyle(1, RED);
                    bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                }
                else
                {
                    // Daca am dat pe langa
                    setfillstyle(1, LIGHTGRAY);
                    bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                }

            }
        }
    }
    // Trage primul jucator
    delay(20);
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        x=mousex();
        y=mousey();

        linia=(y-sus)/latura+1;
        coloana=(x-stanga)/latura+1;
        x1=stanga+latura*(coloana-1);
        y1=sus+latura*(linia-1);
        x2=x1+latura;
        y2=y1+latura;
        xmijloc=(x1+x2)/2;
        ymijloc=(y1+y2)/2;

        setfillstyle(SOLID_FILL,LIGHTGRAY);
        //Conditia pt a nu desena inafara hartii
        if (x >= stanga && y >= sus && (x < stanga + (10 * latura )) && (y < sus + (10 * latura)) )
        {
            if (Player2_harta[linia][coloana] && contor)
            {
                setfillstyle(1, RED);
                bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                Player1_shoot[linia][coloana] = 1;
                DimNava = AfluDimNavei(Player2_harta, linia, coloana);
                ordine = 0;
                EvidentaStariiAdversarului (DimNava, ordine);
                contor--;
//                PlaySound(TEXT("explosionHit.wav"),NULL,SND_SYNC);

            }
            else
                if (!Player2_harta[linia][coloana] && contor)
            {
                setfillstyle(1, LIGHTGRAY);
                bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                Player1_shoot[linia][coloana] = 1;
                contor--;
  //              PlaySound(TEXT("explosionWater.wav"),NULL,SND_SYNC);
            }
        }
    }
    Restrictii(contor);
}
void MarcheazaLovituriSecund ()
{
    int linia,coloana,x,y;
    int x1, y1, x2, y2;
    int xmijloc, ymijloc;
    for (int i =1 ; i <= 10; i++)
    {
        for (int j = 1; j <= 10; j++)
        {
            x1=stanga+latura*(j-1);
            y1=sus+latura*(i-1);
            x2=x1+latura;
            y2=y1+latura;
            xmijloc=(x1+x2)/2;
            ymijloc=(y1+y2)/2;

            if (Player2_shoot[i][j])
            {
                if (Player1_harta[i][j])
                {
                    setfillstyle(1, RED);
                    bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                }
                else
                {
                    setfillstyle(1, LIGHTGRAY);
                    bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                }

            }
        }
    }
    delay(20);
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        x=mousex();
        y=mousey();

        linia=(y-sus)/latura+1;
        coloana=(x-stanga)/latura+1;
        x1=stanga+latura*(coloana-1);
        y1=sus+latura*(linia-1);
        x2=x1+latura;
        y2=y1+latura;
        xmijloc=(x1+x2)/2;
        ymijloc=(y1+y2)/2;
        setfillstyle(SOLID_FILL,LIGHTGRAY);
        //Conditia pt a nu desena inafara hartii
        if (x >= stanga && y >= sus && (x < stanga + (10 * latura )) && (y < sus + (10 * latura)) )
        {
            if (Player1_harta[linia][coloana] && contor1)
            {
                setfillstyle(1, RED);
                bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                Player2_shoot[linia][coloana] = 1;
                DimNava = AfluDimNavei(Player1_harta, linia, coloana);
                ordine = 1;
                EvidentaStariiAdversarului (DimNava, ordine);
                contor1--;
//                PlaySound(TEXT("explosionHit.wav"),NULL,SND_SYNC);
            }
            else
                if (!Player1_harta[linia][coloana] && contor1)
            {
                setfillstyle(1, LIGHTGRAY);
                bar(xmijloc-latura/2+6,ymijloc-latura/2+6,xmijloc+latura /2+2,ymijloc+latura/2+2);
                Player2_shoot[linia][coloana] = 1;
                contor1--;
  //              PlaySound(TEXT("explosionWater.wav"),NULL,SND_SYNC);
            }
        }
    }
    Restrictii(contor1);
}
void GamerShoot(int &work)
{
    clearviewport();
    setbkcolor(CYAN);

    setcolor(BLUE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    if (work % 2 != 0)
        outtextxy(15, ScreenHeight / 15, (char *)"Primul Jucator trage:");
    else
    {
        outtextxy(15, ScreenHeight / 15,(char *) "Al doilea Jucator ");
        outtextxy(15, ScreenHeight / 8,(char *) "trage: ");
    }


    numar = 10;
    width = 600;
    height = 500;
    latura = width / numar;
    sus=(getmaxy()-width) / 3;
    stanga=(getmaxx()- (2 * height - 100));
    // Desenez matricea
    for (int k = 1; k <= 5; k++)
        for(int i=1; i<=numar; i++)
        {
            for(int j=1; j<=numar; j++)
                rectangle(stanga+latura*(i-1)+k,sus+latura*(j-1)+k,stanga+latura*i+k,sus+latura*j+k);
        }
    char arr[20];
    char coord_top[20];
    int nr = 1;
    int poz_lit = 65;
    for (int i = 1; i <= numar; i++)
    {
        switch(poz_lit)
        {
        case 65:
            strcpy (coord_top, "A");
            break;
        case 66:
            strcpy (coord_top, "B");
            break;
        case 67:
            strcpy (coord_top, "C");
            break;
        case 68:
            strcpy (coord_top, "D");
            break;
        case 69:
            strcpy (coord_top, "E");
            break;
        case 70:
            strcpy (coord_top, "F");
            break;
        case 71:
            strcpy (coord_top, "G");
            break;
        case 72:
            strcpy (coord_top, "H");
            break;
        case 73:
            strcpy (coord_top, "I");
            break;
        case 74:
            strcpy (coord_top, "J");
            break;

        }
        sprintf(arr, "%d", nr);
        if (nr != 10)
            outtextxy(stanga - 20, sus+latura*(i-1) + 6, arr );
        else
            outtextxy(stanga - 40, sus+latura*(i-1) + 7, arr );
        delay (5);
        outtextxy ( stanga+latura*(i-1) + 5, sus-30, coord_top );
        poz_lit++;
        nr++;
    }
    if (work % 2 != 0)
    {
        contor = stare.nrDeLov1;
        do
        {
            MarcheazaLovituriPrim();
        }
        while (!GetAsyncKeyState(VK_RETURN));
    }
    else
    {
        contor1 = stare.nrDeLov2;
        do
        {
            MarcheazaLovituriSecund();
        }
        while (!GetAsyncKeyState(VK_RETURN));

    }
    work++;
}
void Draw_Harta (int ordine)
{
    setbkcolor(CYAN);
    clearviewport();

    setcolor(BLUE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 6);
    if (ordine == 1)     outtextxy(15, ScreenHeight / 10,(char *) "Primul Jucator");
    else                 outtextxy(15, ScreenHeight / 10,(char *) "Al doilea Jucator");

    //setcolor(WHITE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    outtextxy(15, ScreenHeight / 5,(char *)"Posibilitati de asezare " );
    outtextxy(15, ScreenHeight/4, (char *)"a navelor :");

    outtextxy(15, ScreenHeight/3, (char *) "1)");
    delay(5);
    outtextxy(15, ScreenHeight/3 + ScreenHeight / 8, (char *)"2)");
    delay(5);
    outtextxy(15, ScreenHeight/2 + ScreenHeight / 10,(char *) "3)  2*");

    /*
    rectangle(-2 , -2 , ScreenWidth , ScreenHeight);
    setfillstyle(10 , 1 );
    floodfill(1 , 1 , WHITE);
    */
    for(int j = 1; j <= 5; j++)
        for (int i = 70; i <= 280; i = i + 70)
            rectangle(i +j, ScreenHeight/3 - 20 +j, i + 70+j, ScreenHeight/3 + 50+j );

    for(int j = 1; j <= 5; j++)
        for (int i = 70; i <= 210; i = i + 70)
            rectangle(i+j,ScreenHeight/3 + ScreenHeight / 9 +j, i + 70 +j, ScreenHeight/3 + ScreenHeight / 8 + 50 +j );

    for(int j = 1; j <= 5; j++)
        for (int i = 140 ; i<= 210; i = i + 70)
            rectangle(i +j, ScreenHeight/2 + ScreenHeight / 12 +j, i + 70 +j,ScreenHeight/2 + ScreenHeight / 10 + 45 +j );

    numar = 10;
    width = 650;
    height = 500;
    latura = width / numar;
    sus=(getmaxy()-width) / 3;
    stanga=(getmaxx()- (3*height /2 )) ;

    for (int k = 1; k <= 5; k++)
        for(int i=1; i<=numar; i++)
        {
            for(int j=1; j<=numar; j++)
                rectangle(stanga+latura*(i-1)+k,sus+latura*(j-1)+k,stanga+latura*i+k,sus+latura*j+k);
        }
    char arr[20];
    char coord_top[20];
    int nr = 1;
    int poz_lit = 65;
    for (int i = 1; i <= numar; i++)
    {
        switch(poz_lit)
        {
        case 65:
            strcpy (coord_top, "A");
            break;
        case 66:
            strcpy (coord_top, "B");
            break;
        case 67:
            strcpy (coord_top, "C");
            break;
        case 68:
            strcpy (coord_top, "D");
            break;
        case 69:
            strcpy (coord_top, "E");
            break;
        case 70:
            strcpy (coord_top, "F");
            break;
        case 71:
            strcpy (coord_top, "G");
            break;
        case 72:
            strcpy (coord_top, "H");
            break;
        case 73:
            strcpy (coord_top, "I");
            break;
        case 74:
            strcpy (coord_top, "J");
            break;
        }
        sprintf(arr, "%d", nr);
        if (nr != 10)
            outtextxy(stanga - 20, sus+latura*(i-1) + 6, arr );
        else
            outtextxy(stanga - 40, sus+latura*(i-1) + 7, arr );
        delay (5);
        outtextxy ( stanga+latura*(i-1) + 5, sus-30, coord_top );
        poz_lit++;
        nr++;
    }
    do
    {
        put_in_matrix();
    }
    while (!GetAsyncKeyState(VK_RETURN));
    urm = ordine + 1;
    if (urm <= 2)
    {
        for (int i = 1; i <= 10; i++)
        {
            for (int j = 1; j <= 10; j++)
                cout << Player1_harta[i][j] << ' ';
            cout << endl ;
        }
        cout << endl;
        delay(50);
        Draw_Harta(urm);
    }
    else
    {
        for (int i = 1; i <= 10; i++)
        {
            for (int j = 1; j <= 10; j++)
                cout << Player2_harta[i][j] << ' ';
            cout << endl;
        }
        continua = 1;
        while (continua)
        {
            if (GetAsyncKeyState(VK_RETURN))
                GamerShoot(ordineJucator);
        }
    }
}

void Draw_Instructiuni()
{
    cleardevice();
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    setcolor(WHITE);
    outtextxy(80, 80,(char *)"Inapoi" );
    outtextxy(300, 30,(char *)"Instructiuni:" );
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy( 350, 70,(char *)"-Primul jucator isi aseaza navele . Una de 4 casute , una de 3 casute si " );
    outtextxy( 350, 90,(char *)"doua de 2 casute" );

    //design punctat
	rectangle(-2, -2, ScreenWidth, ScreenHeight);
    setfillstyle(10, 1 );
    floodfill(1, 1, WHITE);
    setcolor(BLUE);

    //cadru fereastra Inapoi
    rectangle((ScreenWidth - ScreenHeight) / 12, 50,(ScreenWidth - ScreenHeight) / 3, 150);
}

void draw_Meniu()
{
    setcolor(WHITE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 8);
    outtextxy((ScreenWidth - ScreenHeight) / 2 - (ScreenWidth - ScreenHeight) / 10, 30,(char *)" BattleSHIP Project" );

    settextstyle(BOLD_FONT, HORIZ_DIR, 9);
    outtextxy( (ScreenWidth - ScreenHeight) / 2 + ( ScreenWidth - ScreenHeight) / 8 +20,ScreenHeight / 5 + 10,(char*) "Start Game" );

    settextstyle(BOLD_FONT, HORIZ_DIR, 9);
    outtextxy((ScreenWidth - ScreenHeight) / 2 + (ScreenWidth / 6) + 20,(ScreenHeight /2 ) + (ScreenHeight / 8) + 10,(char *) "Exit");

    settextstyle(BOLD_FONT, HORIZ_DIR, 9);
    outtextxy((ScreenWidth - ScreenHeight) / 4 + (ScreenWidth / 6),(ScreenHeight / 2) - (ScreenHeight /10) + 20,(char *) "Instructiuni" );

    rectangle(-2, -2, ScreenWidth, ScreenHeight);
    setfillstyle(10, 1 );
    floodfill(1, 1, WHITE);
    // Desenare cadru Start game
    for (int i = 1; i <= 10; i= i +1)
    {
        setcolor(1);
        rectangle((ScreenWidth - ScreenHeight) / 2 - 15 + i, ScreenHeight / 5 + i, (ScreenWidth / 2) + (ScreenWidth / 4)+ i, (ScreenHeight / 2 ) - (ScreenHeight/6) + i);
    }
    // Desenare cadru Exit
    for (int i = 1; i <= 10; i++)
        rectangle((ScreenWidth - ScreenHeight) / 2 + i, (ScreenHeight / 2) - (ScreenHeight / 10) + i,ScreenWidth / 2 + ScreenWidth /4   + i, (ScreenHeight / 2) + (ScreenHeight / 20) + i );
    // Desenare cadru Instructiuni
    for (int i = 1; i <= 10; i++)
        rectangle((ScreenWidth - ScreenHeight) / 2 + (ScreenWidth / 10) + i, (ScreenHeight /2 ) + (ScreenHeight / 4) + i,ScreenWidth / 2 + ScreenWidth /8  + i, (ScreenHeight / 2) + (ScreenHeight / 10) + i );

    // Limitele pentru fereastra Exit
    int xLimInf2 = (ScreenWidth - ScreenHeight) / 2 + (ScreenWidth / 10);
    int yLimInf2 = (ScreenHeight / 2) - (ScreenHeight /10);
    int xLimSup2 = ScreenWidth / 2 + ScreenWidth /8 ;
    int yLimSup2 = (ScreenHeight / 2) + (ScreenHeight / 20);

    // Limitele pentru fereastra Start Game
    int xLimInf1 = (ScreenWidth - ScreenHeight) / 2 ;
    int yLimInf1 = ScreenHeight / 5;
    int xLimSup1 = ScreenWidth / 2 + ScreenWidth / 4;
    int yLimSup1 = ScreenHeight/2-ScreenHeight/6 ;

    // Limitele pentru fereastra Instructiuni
    int xLimInf = (ScreenWidth - ScreenHeight) / 2;
    int yLimInf = (ScreenHeight / 2 ) + ScreenHeight / 12;
    int xLimSup = ScreenWidth / 2 + ScreenWidth / 4 ;
    int yLimSup = (ScreenHeight / 2) +(ScreenHeight / 3);

    // Limitele pentru fereastra Inapoi din instructiuni
    int xLimInf3 = (ScreenWidth - ScreenHeight) / 12;
    int yLimInf3 = 50;
    int xLimSup3 = (ScreenWidth - ScreenHeight) / 3;
    int yLimSup3 = 150;

    while (ok)
    {
        //e apasat butonul Exit
        if (GetAsyncKeyState(VK_LBUTTON) && mousex()>xLimInf && mousey() > yLimInf && mousex() <xLimSup && mousey() < yLimSup )
        {
//            PlaySound(TEXT("buttonPress.wav"),NULL,SND_SYNC);
            ok = 0;
        }
        //e apasat butonul Start game
        if (GetAsyncKeyState(VK_LBUTTON) && mousex()>xLimInf1 && mousey() > yLimInf1 && mousex() <xLimSup1 && mousey() < yLimSup1 )
        {
  //          PlaySound(TEXT("buttonPress.wav"),NULL,SND_SYNC);
            clearmouseclick(WM_LBUTTONDOWN);
            Draw_Harta(nr_de_ordine);
        }
        //e apasat butonul Instructiuni
        if (GetAsyncKeyState(VK_LBUTTON) && mousex()>xLimInf2 && mousey() > yLimInf2 && mousex() <xLimSup2 && mousey() < yLimSup2 )
        {
 //           PlaySound(TEXT("buttonPress.wav"),NULL,SND_SYNC);
            Draw_Instructiuni();
        }
        //e apasat butonul Inapoi din instructiuni
        if (GetAsyncKeyState(VK_LBUTTON) && mousex()>xLimInf3 && mousey() > yLimInf3 && mousex() <xLimSup3 && mousey() < yLimSup3 )
        {
 //           PlaySound(TEXT("buttonPress.wav"),NULL,SND_SYNC);
            cleardevice();
            //init_Harta();
            draw_Meniu();
        }
       // delay(300);
    }
}
int main ()
{
    init_Harta();
    init_structura();
    draw_Meniu();
    if (ok == 0)
        closegraph();
    return 0;
}
