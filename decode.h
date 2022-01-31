#ifndef ENCODE_H
#define ENCODE_H
#include <stdio.h>
#include <string.h>
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{
    /* Sourc Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;

    /* Secret File Info */
    int ext_size;
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
   // char secret_data[MAX_SECRET_BUF_SIZE];
    char secret_data;
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];
    char magic_string[2];

}DecodeInfo;

/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_image(DecodeInfo *decInfo);


/* decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Encode file extention size */
Status decode_size(int size, DecodeInfo *decInfo);

 //Encode a extention size into LSB of image data array
Status decode_ext_size_from_lsb(int size, char *image_buffer,DecodeInfo *decInfo);

// Encode secret file extenstion 
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Encode secret file size 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

Encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo);

 Encode function, which does the real encoding */
Status decode_data_from_image(int size,char *decoded_data, DecodeInfo *decInfo);

// Encode a byte into LSB of image data array 
Status decode_byte_from_lsb(char *image_buffer,DecodeInfo *decInfo);

/* Copy remaining image bytes from src to stego image after encoding 
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
*/
#endif
