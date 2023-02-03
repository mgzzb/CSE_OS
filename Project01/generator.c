#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Generate a forward alphabetical string 
   @param   nRepeats  The number of times to repeat the string (0 means only one iteration)
   @returns Populated string including a null character. It is the callees responsibility to free
*/

char * generateAlphaForward (int nRepeats)
{
    int     j;
    char *  pOutBuffer;
    int     nBaseSize;
    int     nBufferSize;

    if(nRepeats < 0)
    {
        printf("< 0 is not allowed, skedaddle please ....\n");
        return NULL;
    }

    /* Go forward and multiply into a buffer please */

    /* Allocate it */

    /* Slightly ancient magic below */
    /*   This is just using the actual value of the characters 
          https://www.asciitable.com  
          A = 65
          z = 122

          Key Note: Single quotes is the character (ASCII) 
    */
    nBaseSize = (int) 'z' - (int) 'A' + 1;

    printf("ASCII Conversions: \n");
    printf("   %c  is %d  0x%X\n", 'A', 'A', 'A');
    printf("   %c  is %d  0x%X\n", 'z', 'z', 'z');

    nBufferSize = nBaseSize * (nRepeats + 1);

    printf(" Allocated %d bytes\n", nBufferSize);

    /* And space for the null */
    nBufferSize += 1;

    pOutBuffer = (char *) malloc (nBufferSize);

    /* Did we have to be that fancy --> nope */
    /*   We could just have looked in the table and figured it out by hand / written
         it in the code along with an explanation 
     */

    printf("Let's loop and add: \n");
    for(j=0; j<  nBaseSize * (nRepeats + 1); j++)
    {
        pOutBuffer[j] = (j % nBaseSize) + (char) 'A';
        printf("Buffer spot [%d]: %c\n", j, (char) (j % nBaseSize + 'A'));
    }
    printf("\n");

    /* Add the null terminator */
    pOutBuffer[nBufferSize-1] = '\0';

    return pOutBuffer;
}


char * generateDecoderMessage ()
{
    char * pszBuffer;

    pszBuffer = strdup("Drink more ovaltine");

    for(int j=0; j<strlen(pszBuffer); j++)
    {
        pszBuffer[j] = pszBuffer[j] ^ 0xFF;
    }

    return pszBuffer;
}


void generateExamples ()
{
    FILE * pFile;
    char * pBuffer;
    int     nBufferSize;


    pFile = fopen("AlphaSingle.txt", "w");
    pBuffer = generateAlphaForward(0);
    nBufferSize = 122 - 65 + 1;
    fwrite(pBuffer, nBufferSize, 1, pFile);
    printf("Wrote %d bytes to AlphaSingle\n", nBufferSize);
    fclose(pFile);
    free(pBuffer);

    pFile = fopen("AlphaDouble.dat", "w");
    pBuffer = generateAlphaForward(1);
    nBufferSize = (122 - 65) * 2 + 2;
    fwrite(pBuffer, nBufferSize, 1, pFile);
    printf("Wrote %d bytes to AlphaDouble\n", nBufferSize);
    fclose(pFile);
    free(pBuffer);

    pFile = fopen("SecretMessage.enc", "w");
    pBuffer = generateDecoderMessage();
    nBufferSize = strlen("Drink more ovaltine");
    fwrite(pBuffer, nBufferSize, 1, pFile);
    fclose(pFile);
    free(pBuffer);
}


int main (int argc, char *argv[])
{
    generateExamples();
    return 0;
}





