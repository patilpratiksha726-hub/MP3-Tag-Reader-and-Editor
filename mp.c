#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mp.h"

void help()
{
    printf("------------------------HELP MENU------------------------\n");
    printf("1. -v -> to view mp3 file contents\n");
    printf("2. -e -> to edit mp3 file contents\n");
    printf("    2.1. -t -> to edit song title\n");
    printf("    2.2. -a -> to edit artist name\n");
    printf("    2.3. -A -> to edit album name\n");
    printf("    2.4. -y -> to edit year\n");
    printf("    2.5. -m -> to edit content\n");
    printf("    2.6. -c -> to edit comment\n");
    printf("--------------------------------------------------------\n");
}

void view(FILE *fptr)
{
    fseek(fptr, 5, SEEK_CUR);
    char frame[5];
    int i = 0;
    printf("********************** VIEW MENU ***********************\n");
    while (i < 6)
    {
        if (fread(frame, 1, 4, fptr) != 4)
        {
            break;
        }
        frame[4] = '\0';
        if (frame[0] == '\0')
        {
            break;
        }
        unsigned char big_size[4];
        if (fread(big_size, 1, 4, fptr) != 4)
            break;

        int size = (big_size[0] << 24) |
                   (big_size[1] << 16) |
                   (big_size[2] << 8) |
                   big_size[3];
        if (size <= 0)
        {
            break;
        }
        fseek(fptr, 2, SEEK_CUR);
        char *contain = malloc(size + 1); // the extra one space for the eencoding format
        if (contain == NULL)
        {
            printf("Memory its not allocating.");
            break;
        }
        fread(contain, 1, size, fptr);
        contain[size] = '\0';
        char print[50];
        if (strcmp(frame,"TPE1")==0)
        {
            strcpy(print, "Artist name");
        }
        else if (strcmp(frame,"TIT2")==0)
        {
            strcpy(print, "Title name");
        }
        else if (strcmp(frame,"TALB")==0)
        {
            strcpy(print, "Album");
        }
        else if (strcmp(frame,"TYER")==0)
        {
            strcpy(print, "Year");
        }
        else if (strcmp(frame,"TCON")==0)
        {
            strcpy(print, "Content type");
        }
        else if (strcmp(frame,"TCOM")==0)
        {
            strcpy(print, "Composer");
        }
        print[strlen(print)+1]='\0';
        
        

        printf("%-15s : %s\n", print, contain + 1); // starting from contain + 1 for the frist byte for the text encoding;

        free(contain);
        i++;
    }
    fclose(fptr);
    printf("*******************************************************\n");
}

void edit(int count, char *str[])
{
    char edit_fream[5];
    char check_fream[5];
    edit_fream[4] = '\0';
    check_fream[4] = '\0';

    int new_size = strlen(str[3]) + 1;

    switch (str[2][1])
    {
    case 't':
        edit_fream[0] = 'T';
        edit_fream[1] = 'I';
        edit_fream[2] = 'T';
        edit_fream[3] = '2';
        break;

    case 'a':
        edit_fream[0] = 'T';
        edit_fream[1] = 'P';
        edit_fream[2] = 'E';
        edit_fream[3] = '1';
        break;

    case 'A':
        edit_fream[0] = 'T';
        edit_fream[1] = 'A';
        edit_fream[2] = 'L';
        edit_fream[3] = 'B';
        break;

    case 'y':
        edit_fream[0] = 'T';
        edit_fream[1] = 'Y';
        edit_fream[2] = 'E';
        edit_fream[3] = 'R';
        break;

    case 'm':
        edit_fream[0] = 'T';
        edit_fream[1] = 'C';
        edit_fream[2] = 'O';
        edit_fream[3] = 'N';
        break;

    case 'c':
        edit_fream[0] = 'C';
        edit_fream[1] = 'O';
        edit_fream[2] = 'M';
        edit_fream[3] = 'M';
        break;
    }

    FILE *fptr1, *fptr2;
    fptr1 = fopen(str[4], "rb");
    if (fptr1 == NULL)
    {
        printf("Cannot open input file.\n");
        return;
    }
    fptr2 = fopen("output.mp3", "wb");
    if (fptr2 == NULL)
    {
        printf("Cannot open output file.\n");
        return;
    }
    char ch;
    for (int i = 1; i <= 10; i++)
    {
        ch = getc(fptr1);
        putc(ch, fptr2);
    }

    while ((fread(check_fream, 1, 4, fptr1)) == 4) // Checking for the copying the complete fream ID
    {
        if (check_fream[0] == '\0')
        {
            break;
        }
        unsigned char size_bytes[4];
        unsigned char flags[2];

        fread(size_bytes, 1, 4, fptr1);
        fread(flags, 1, 2, fptr1);

        int size = (size_bytes[0] << 24) |
                   (size_bytes[1] << 16) |
                   (size_bytes[2] << 8) |
                   (size_bytes[3]);

        if (memcmp(check_fream, edit_fream, 4) == 0) // Compaire byt by byte
        {
            fseek(fptr1, size, SEEK_CUR);

            fwrite(check_fream, 1, 4, fptr2);

            unsigned char new_size_bytes[4] = {
                (new_size >> 24) & 0xFF,
                (new_size >> 16) & 0xFF,
                (new_size >> 8) & 0xFF,
                new_size & 0xFF};

            fwrite(new_size_bytes, 1, 4, fptr2);
            fwrite(flags, 1, 2, fptr2);

            // Adding the excoding bit
            unsigned char encoding = 0x00;
            fwrite(&encoding, 1, 1, fptr2);

            fwrite(str[3], 1, new_size - 1, fptr2);
        }
        else
        {
            fwrite(check_fream, 1, 4, fptr2);
            fwrite(size_bytes, 1, 4, fptr2);
            fwrite(flags, 1, 2, fptr2);

            unsigned char *fream_details = malloc(size);
            if (fream_details == NULL)
            {
                break;
            }
            fread(fream_details, 1, size, fptr1);

            fwrite(fream_details, 1, size, fptr2);

            free(fream_details);
        }
    }
    fclose(fptr1);
    fclose(fptr2);

    if (remove(str[4]) != 0)
    {
        printf("Error removing original file\n");
    }

    if (rename("output.mp3", str[4]) != 0)
    {
        printf("Error renaming file\n");
    }
    else
    {
        printf("Tag edited successfully.\n");
    }
}
