#include "encode.h"
#include "types.h"
#include "common.h"

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
    printf("INFO:Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    printf("INFO:Opened %s\n\n",encInfo -> src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }
    printf("INFO:Opened %s\n",encInfo -> secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }
    printf("INFO:Opened %s\n\n",encInfo -> stego_image_fname);

    // No failure return e_success
    return e_success;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    uint size;
    //seek to end of the file
    fseek(fptr,0,SEEK_END);
    //getting the size
    size = ftell(fptr);
    //returning size
    return size;
}

/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    /* Checking for -e */
    if (argv[1] != NULL)
	if (!(strcmp(argv[1],"-e")))
	    return e_encode;
    /* Checking for -d */
	else if (!(strcmp(argv[1],"-d")))
	    return e_decode;
    return e_unsupported;

}
/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //cvalidating .bmp file
    if (strstr(argv[2],".bmp"))
	encInfo -> src_image_fname = argv[2];
    else
    {
	fprintf(stderr,"ERROR:%s is not a .bmp file\n",argv[2]);
	return e_failure;
    }
    //validating for secret file extentions
    if (strstr(argv[3],".txt"))
	encInfo -> secret_fname = argv[3];
    else
    {
	fprintf(stderr,"ERROR:%s is not a .txt file\n",argv[3]);
	return e_failure;
    }
    //checking whether the 4th argument is passedd or not
    if (argv[4] != NULL)
    {
	//assigning the 4th argument as encoded name
	encInfo -> stego_image_fname = argv[4];
    }
    else
	encInfo -> stego_image_fname = "stego.bmp";

    return e_success;
}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    /*opening files*/
    if ((open_files(encInfo)) == e_success)
    {
	printf("INFO:Done\n");
	//checking the capacity
	if ((check_capacity(encInfo)) == e_success)
	{
	    printf("INFO:Done\n");
	    rewind(encInfo -> fptr_stego_image);
	    rewind(encInfo -> fptr_src_image);
	    printf("INFO:Cheking for %s size\n",encInfo -> secret_fname);
	    if (encInfo -> size_secret_file != 0)
	    {
		printf("INFO:Done.Not empty\n");
	    }
	    else
	    {
		printf("INFO:%s is empty\n",encInfo -> secret_fname);
	    }

	    //copying the header data into new stego image
	    if ( copy_bmp_header(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
	    {
		printf("INFO:Done\n");
		//encoding the magic string
		if (encode_magic_string(MAGIC_STRING,encInfo) == e_success)
		{
		    printf("INFO:Done\n");
		    //encoding the extension size
		    if (encode_size(strlen(".txt"), encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
		    {

			//encoding the file extention 
			if(encode_secret_file_extn(".txt", encInfo) == e_success)
			{
			    printf("INFO:Done\n");

			    //encoding the secret file size
			    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
			    {
				printf("INFO:Secret File size encoding successfull\n");
				//encoding the secret file data
				if(encode_secret_file_data(encInfo) == e_success)
				{
				    printf("INFO:Done\n");

				    //copying the remaining data
				    if( copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
				    {
					printf("## Encoding Done Succesfully ##\n");

				    }
				    else
				    {
					printf("ERROR:Complete data encoding failure\n");
					return e_failure;

				    }
				}
				else
				{
				    printf("ERROR:secret data encoding failure\n");
				    return e_failure;

				}

			    }
			    else
			    {
				printf("ERROR:file size encoding failure\n");
				return e_failure;

			    }

			}
			else
			{
			    printf("ERROR:Extension extention encoding failure\n");
			    return e_failure;

			}

		    }
		    else
		    {
			printf("ERROR:FIle extention size encoding failure\n");
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
    printf("INFO:Checking for %s capacity to handle %s\n",encInfo -> src_image_fname,encInfo -> secret_fname );
    //storing the size using functions
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    //rewinding the pointer
    rewind(encInfo -> fptr_secret);

    if (encInfo -> size_secret_file != 0)
	//checking the capacity
	if (encInfo -> image_capacity > (54 + 16 + 32 + 32 + 32 + (8 * encInfo -> size_secret_file)))
	    return e_success;
}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO:Copying Image Header\n");
    char ch;
    //rewinding the pointer to source image
    rewind(fptr_src_image);
    //copying the first 54 bytes
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
    printf("INFO:Encoding Magic String signature\n");
    /* Encode function, which does the real encoding */
    if (encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
    {
	return e_success;
    }
    else
	return e_failure;

}

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{

    char image_data[8];
    for(int i = 0;i < size;i++)
    {
	//storing 8 bytes for source image
	fread(image_data,8,1,fptr_src_image);
	/* Encode a byte into LSB of image data array */
	encode_byte_to_lsb(data[i],image_data);
	//writing the encoded data into stego image
	fwrite(image_data,8,1,fptr_stego_image);
    }
    return e_success;
}


/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //clearing lsb bit of each bytes and storig each bit to the data
    for(int i = 0;i < 8;i++)
	image_buffer[i] = (image_buffer[i] & 0XFE) | ((data >> (7 - i)) & 1);
    return e_success;
}

/* Encode file extention size*/
Status encode_size(int size, FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char image_data[32];
    //getting 32 bytes from source image
    fread(image_data,32,1,fptr_src_image);
    //encoding intiger value of size into 32 bytes
    encode_ext_size_to_lsb(size, image_data);
    //writing the size into stegno image
    fwrite(image_data,32,1,fptr_stego_image);
    return e_success;


}

/* Encode a extention size into LSB of image data array */
Status encode_ext_size_to_lsb(int size, char *image_buffer)
{
    //encoding the lb bits with the intiger size
    for(int i = 0;i < 32;i++)
    {
	image_buffer[i] = (image_buffer[i] & 0XFE) | ((size >> (31 - i)) & 1);
    }
    return e_success;
}
/* Encode secret file extenstion */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO:Encoding %s File extention\n",encInfo -> secret_fname);

    //encoding data into image
    if (encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo ->fptr_stego_image) == e_success)
	return e_success;
    else
	return e_failure;
}



/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO:Encoding %s File size\n",encInfo -> secret_fname);
    //encoding the size
    if (encode_size(file_size, encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
	return e_success;
    else
	return e_failure;
}


/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char data;
    int size = encInfo -> size_secret_file;
    for (int i = 0;i < size;i++)
    {
	//reading the image data
	fread(encInfo -> image_data,8,1,encInfo -> fptr_src_image);
	//reading the data
	fread(&data,1,1,encInfo -> fptr_secret);
	//encoding to the lsb bits
	encode_byte_to_lsb(data, encInfo -> image_data);
	//writing the encoded data into stego image
	fwrite(encInfo -> image_data,8,1,encInfo -> fptr_stego_image);
    }
    return e_success;

}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO:Copying left over data\n");
    char ch;
    //copying the remaining data
    while (fread(&ch,1,1,fptr_src))
	fwrite(&ch,1,1,fptr_dest);
    return e_success;


}











