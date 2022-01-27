#include "encode.h"
#include "types.h"

/* Function Definitions */
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    uint size;
    fseek(fptr,0,SEEK_END);
    size = ftell(fptr);
    return size;
}

/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    /* Checking for -e */
    if (!(strcmp(argv[1],"-e")))
	return e_encode;
    /* Checking for -d */
    else if (!(strcmp(argv[1],"-d")))
	return e_decode;
    else
	return e_unsupported;
}
/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strstr(argv[2],".bmp"))
	encInfo -> src_image_fname = argv[2];
    else
    {
	fprintf(stderr,"ERROR:%s is not a .bmp file\n",argv[2]);
	return e_failure;
    }
    if (strstr(argv[3],".txt"))
	encInfo -> secret_fname = argv[3];
    else
    {
	fprintf(stderr,"ERROR:%s is not a .txt file\n",argv[3]);
	return e_failure;
    }
    if (argv[4] != NULL)
	encInfo -> stego_image_fname = argv[4];
    else
	encInfo -> stego_image_fname = "stego.bmp";

}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if ((open_files(encInfo)) == e_success)
    {
	printf("INFO:Opened %s\nINFO:Opened %s\nINFO:Opened %s\n",encInfo -> src_image_fname,encInfo -> secret_fname,encInfo -> stego_image_fname);

	if ((check_capacity(encInfo)) == e_success)
	{
	    printf("INFO:Check capacity done\n");
	   if ( copy_bmp_header(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
	   {
	       printf("INFO:Copy .bmp header success\n");
	       if (encode_magic_string(MAGIC_STRING,&encInfo) == e_success)
	       {
		   printf("INFO:Magic_string Encoding succesfull\n");
		   if (encode_size(strlen(".txt"), encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_Success)
		   {
		   printf("INFO:Extension size encoding successfull\n");
		  
		  
		   if()
		   {
		   printf("INFO:Secret File extension encoding successfull\n");

		   }
		   else
		   {
		   printf("ERROR:Extension size encoding failure\n");
		   return e_failure;

		   }

	       }
	       else
	       {
		   printf("ERROR:Magic string encoding failure\n");
		   return e_failure;
	       }


	   }
	   else
	   {
	    printf("ERROR:copy bmp header failure\n");
	    return e_failure;
	   }

	}
	else
	{
	    printf("ERROR:Check capaticity id failure\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR:Files not opened\n");
	return e_failure;
    }


}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    
    if (encInfo -> image_capacity > (54 + 16 + 32 + 32 + 32 + (8 * encInfo -> size_secret_file)))
	return e_success;
}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char ch;
    rewind(fptr_src_image);
    for (int i = 0;i < 54;i++)
    {
	ch = fgetc(fptr_src_image);
	fputc(ch,fptr_dest_image);
    }
    return e_success;

}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    /* Encode function, which does the real encoding */
    if (encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_Success)
    {
	return e_success;
    }
    else
	return e_failure;

}

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    for(int i = 0;i < size;i++)
    {
	fread(encInfo -> image_data,8,1,encInfo -> fptr_src_image);
	/* Encode a byte into LSB of image data array */
	encode_byte_to_lsb(data[i], encInfo -> image_data);
	fwrite(encInfo -> image_data,8,1,encInfo -> fptr_stego_image);
    }
    return e_success;
}


/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0;i < 8;i++)
    {
	image_buffer[i] &= 0xFE;
	image_buffer[i] &= ((data >> (7 - i)) & 1);
    }
    return e_success;
}

/* Encode file extention size*/
status encode_size(int size, FILE *fptr_src_image,FILE *fptr_stego_image)
{
    fread(encInfo -> image_data,8,1,encInfo -> fptr_src_image);
    encode_ext_size_to_lsb(sizeof(size), encInfo -> image_data);
    fwrite(encInfo -> image_data,8,1,encInfo -> fptr_stego_image);
    return e_success;


}

/* Encode a extention size into LSB of image data array */
Status encode_ext_size_to_lsb(int size, char *image_buffer)
{
    for(int i = 0;i < size;i++)
    {
	image_buffer[i] &= 0xFE;
	image_buffer[i] &= ((size >> (31 - i)) & 1);
    }
    return e_success;
}
/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{

}









/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);













