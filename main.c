    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "mp.h"



    int main(int count, char *str[])
    {
        // Validation
        if (count == 1)
        {
            printf("------------------------HELP MENU------------------------\n");
            printf("ERROR: ./a.out : INVALID ARGUMENTS\nUSAGE :\nTo view please pass like: ./a.out -v mp3filename\nTo edit please pass like : ./a.out -e -t/-a/-A/-m/-y/-c changing_text mp3filename\nTo get help pass like : ./a.out -help\n");
            printf("--------------------------------------------------------\n");
            return 0;
        }

        if (strcmp(str[1], "-v") != 0 &&
            strcmp(str[1], "-e") != 0 &&
            strcmp(str[1], "-h") != 0 &&
            strcmp(str[2], "-t") != 0 &&
            strcmp(str[2], "-a") != 0 &&
            strcmp(str[2], "-A") != 0 &&
            strcmp(str[2], "-y") != 0 &&
            strcmp(str[2], "-m") != 0 &&
            strcmp(str[2], "-c") != 0)
        {
            printf("Invalid Operation.\n");
            return 0;
        }
        if (count == 3)
        {
            if (strcasestr(str[2], ".mp3") == NULL)
            {
                printf("Invalid song file!\n");
                return 0;
            }
        }
        else
        {
            if (strcasestr(str[4], ".mp3") == NULL)
            {
                printf("Invalid song file!\n");
                return 0;
            }
        }

        FILE *fptr;
        if (count == 3)
        {
            fptr = fopen(str[2], "rb");
        }
        else
        {
            fptr = fopen(str[4], "rb");
        }

        if (fptr == NULL)
        {
            printf("The File is not opend!\n");
            return 0;
        }

        char check[] = {'I', 'D', '3'};
        int i = 0;
        char ch;
        while (i < 3)
        {
            if ((ch = getc(fptr)) != check[i])
            {
                printf("The data is not found in the mp3 file!");
                fclose(fptr);
                return 0;
            }
            i++;
        }
        unsigned char version[2];
        fread(version, 1, 2, fptr);
        if (version[0] != 3)
        {
            printf("Version is not ID3v2.3\n");
            fclose(fptr);
            return 0;
        }
        printf("Valid ID3 version: 2.%d.%d\n\n", version[0], version[1]);

        if (strcmp(str[1], "-v") == 0)
        {
            view(fptr);
        }
        else if ((strcmp(str[1], "-e") == 0))
        {
            fclose(fptr);

            edit(count, str);
        }
        else if ((strcmp(str[1], "-h") == 0))
        {
            help();
        }
        return 0;
    }