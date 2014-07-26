// testing the filehandle pointer; and this is useful later on in the main program
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(void)
{
    FILE * fstream;
    fstream = fopen("test.txt", "r");
    if(fstream == NULL)
    {
        printf("open file test.txt failed!\n");
        exit(1);
    }
    else
    {
        printf("open file test.txt succeed!\n");
    }

    int len = 50;
    char string[len];
    int i = 0;

    // pre-processing
    fgets(string, len, fstream);
    printf("%s", string);
    while(!feof(fstream))
    {
        i++;
        if(i==4)
        {
            FILE fstream1;  // fstream1 is the struct other than a pointer
            memcpy(&fstream1, fstream, sizeof(*fstream));
            printf("new start here!!\n");
            while(!feof(&fstream1))
            {
                fgets(string, len, &fstream1);
                printf("%s", string);
            }
            //fclose(fstream1);
            printf("end my try here!!\n");
        }
        fgets(string, len, fstream);
        printf("%s", string);
    }

    fclose(fstream);
    return 0;
}