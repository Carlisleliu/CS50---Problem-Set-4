```
#include <cs50.h>
#include <stdio.h>
#include <stdint.h>

// define an alternative term BYTE for 8-bit unsigned number
typedef uint8_t BYTE;

int main (int argc, char *argv[])
{
    // ensure a 2-argument command line input
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover card.\n");
        return 1;
    }
    
    // ensure the memory card provided can be properly opened by the programme
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        fclose(card);
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }
    
    // set the starting jpg file name as 0
    int jpg_num = 0;
    
    // set the size of the character array of the file name
    char jpg_name[10];
    
    // set a memory of size 512 bytes
    BYTE block[512];
    
    // set the temporary output file
    FILE *jpeg = NULL;
    
    
    // execute the code as long as it has not reached the end of memory card
    while (!feof(card))
    {
        // identify the jpeg file
        if (block[0] == 0xff && block[1] == 0xd8 && block[2] == 0xff && (block[3] & 0xf0) == 0xe0)
        {
            // if the programme has been retrieving a jpeg file prior to identifying this new jpeg file, close the previous jpeg file
            if (jpeg != NULL)
            {
                fclose(jpeg);
            }
            
            // sends the formatted output to jpg_name, which is the name of the new jpeg file
            sprintf(jpg_name, "%03i.jpg", jpg_num);
            
            // after using the existing jpeg file number, increase the number by 1 for the next jpeg file
            jpg_num++;
            
            // open the newly created jpeg file with writing mode
            jpeg = fopen(jpg_name, "w");
            
            // write the 512 bytes of memory into the jpeg file
            fwrite(&block, sizeof(block), 1, jpeg);
        }
        // if the jpeg file number is not the default number (0), meaning a jpeg file has been created and is being processed
        else if (jpg_num > 0)
        {
            // thus write the 512 bytes of memory into the file that is bring processed
            fwrite(&block, sizeof(block), 1, jpeg);
        }
        // read the next 512 bytes of the memory card
        fread(&block, sizeof(block), 1, card);
    }
    
    // the above code stops when the end of file is reached, then close the memory card
    fclose(card);
    
    return 0;
}
```
