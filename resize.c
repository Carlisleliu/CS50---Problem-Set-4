```
#include <stdio.h>
#include <cs50.h>

#include "bmp.h"


int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./copy n infile outfile\n");
        return 1;
    }
    
    int factor = atoi(argv[1]);
    
    // ensure the resize factor between 0 and 100
    if (factor > 100 || factor < 1)
    {
        fprintf(stderr, "resize factor must be a positive but smaller or equal to 100");
        return 1;
    }
    
    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];
    
    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }
    
    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    
    // memory the original biWidth and biHeight
    int old_biWidth = bi.biWidth;
    int old_biHeight = bi.biHeight;
    
    // resize the width and height of image by factor n;
    bi.biWidth *= factor;
    bi.biHeight *= factor;
    
    // calculate the padding required for original and resized BMP file
    int old_padding = (4 - (old_biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int new_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // calculate new bisizeimage and bfsize
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + new_padding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write editted BITMAPFILEHEADER into output file
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    
    // write editted BITMAPINFOHEADER into output file
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    
    // iterate through the height of the image
    for (int i = 0, old_abs_biHeight = abs(old_biHeight); i < old_abs_biHeight; i++)
    {
        // ensure that each line is repeated factor times before moving to the next one
        for (int row_count = 0; row_count < factor; row_count++)
        {
            // iterate through each byte of the line
            for (int j = 0; j < old_biWidth; j++)
            {
                // ensure that each byte is repeated factor times before moving to the next one
                for (int col_count = 0; col_count < factor; col_count++)
                {
                    // set a temporary memmory of size of RGBTRIPLE
                    RGBTRIPLE triple;
                    
                    // read RGBTRIPLE from input file
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    // write the read RGBTRIPLE into the output file
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    
                    // if the RGBTRIPLE has not been repeated by factor times, set the pointer 1 RGBTRIPLE position back
                    if (col_count != (factor - 1))
                    {
                        fseek(inptr, -sizeof(RGBTRIPLE), SEEK_CUR);
                    }
                }
            }
            // after finishing resize the line, skip the original padding by set the pointer the size of old padding forward
            fseek(inptr, old_padding, SEEK_CUR);
            
            // then add the new padding
            for (int k = 0; k < new_padding; k++)
            {
                fputc(0x00, outptr);
            }
            
            // if the line has not been repeated by factor times, set the pointer back by the size of line including padding
            if (row_count != (factor - 1))
            {
                fseek(inptr, (-sizeof(RGBTRIPLE) * old_biWidth) - old_padding, SEEK_CUR);
            }
        }
    }
    
    // close the input file
    fclose(inptr);
    
    // close the putput file
    fclose(outptr);
    
    return 0;
}
```
