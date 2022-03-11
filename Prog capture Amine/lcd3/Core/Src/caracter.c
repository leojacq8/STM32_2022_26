/*
 * caracter.c

 */

#include "caracter.h"

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
// fonction qui converti un entier en chaine de caractère
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}
// fonction qui converti un réel en chaine de caractère
void ftoat(float n, char *res, int afterpoint)
{
    // on extrait avant tout la partie entiere
    int ipart = (int)n;

    // on extrait maintenant la partie decimal
    float fpart = n - (float)ipart;

    // on convertie la partie entiere en chaine de caractère
    int i = intToStr(ipart, res, 0);

    // afterpoint correspond au nombre de chiffre apres la virgule souhaiter
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot


        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

