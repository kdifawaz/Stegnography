#include "decode.h"
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

Status open_image(DecodeInfo *decInfo)
{

    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");
    // Do Error handling
    if (decInfo -> fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> stego_image_fname);

	return e_failure;
    }

    fseek(decInfo -> fptr_stego_image,54,SEEK_SET);

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
    if (!(strcmp(argv[1],"-e")))
	return e_encode;
    /* Checking for -d */
    else if (!(strcmp(argv[1],"-d")))
	return e_decode;
    else
	return e_unsupported;
}
/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //validating .bmp file
    if (strstr(argv[2],".bmp"))
	decInfo -> stego_image_fname = argv[2];
    else
    {
	fprintf(stderr,"ERROR:%s is not a .bmp file\n",argv[2]);
	return e_failure;
    }
}

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    /*opening files*/
    if ((open_image(decInfo)) == e_success)
    {
	printf("INFO:Opened %s\n",decInfo -> stego_image_fname);

	//decoding the magic string
	if (decode_magic_string(MAGIC_STRING,decInfo) == e_success)
	{
	    printf("INFO:Magic_string decoding succesfull\n");
	    //encoding the extension size
	    if (decode_size(sizeof(int), decInfo) == e_success)
	    {
		decInfo -> ext_size = decInfo -> _32_bit_size;
		printf("\n\n\nExtr size %d\n",decInfo -> ext_size);
		printf("INFO:Extension size decoding successfull\n");

		//encoding the file extention 

		if(decode_secret_file_extn(decInfo) == e_success)
		{
		    printf("INFO:Secret File extension encoding successfull\n");
		    open_secret(decInfo);
		    //strcpy(decInfo -> secret_fname,"Secret");
		    //strcat(decInfo -> secret_fname,decInfo -> extn_secret_file);
		    printf("name %s\n",decInfo -> secret_fname);

		    //decoding the secret file size
		    if(decode_file_size (sizeof(int),decInfo) == e_success)
		    {
			printf("INFO:Secret File size encoding successfull\n");
			decInfo -> file_size = decInfo -> _32_bit_size;
			printf("\n\n\nfile size %d\n",decInfo -> file_size);
			//decoding the secret file data
			if(decode_secret_file_data(decInfo) == e_success)
			{
			    printf("INFO:Secret File data encoding successfull\n");
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
    //  else
    //  {
    //	printf("ERROR:copy bmp header failure\n");
    //	return e_failure;
}
//}
//else
//{
//  printf("ERROR:Check capaticity id failure\n");
//  return e_failure;
//}


//}
//else
//{
//  printf("ERROR:Files not opened\n");
// return e_failure;
//}
//}

/* Decoding Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    /* Decode function, which does the real decoding */
    decode_data_from_image(strlen(MAGIC_STRING),decInfo -> magic_string,decInfo);
    decInfo -> magic_string[strlen(MAGIC_STRING)] = '\0';
    printf("magic str => %s \n",decInfo -> magic_string);
    if (strcmp(decInfo -> magic_string,MAGIC_STRING) == 0)
	return e_success;
    else
	return e_failure;

}

/* Encode function, which does the real encoding */
Status decode_data_from_image(int size, char *decoded_data,DecodeInfo *decInfo)
{


    for(int i = 0;i < size;i++)
    {
	//storing 8 bytes for source image
	fread(decInfo -> image_data,8,1,decInfo -> fptr_stego_image);
	/* decode a LSB of 8 bytes of image data */

	decode_byte_from_lsb(decInfo -> image_data,decInfo);
	decoded_data[i] = decInfo -> secret_data;
    }
    return e_success;
}


/* decode a byte into a charecter*/
Status decode_byte_from_lsb(char *image_buffer,DecodeInfo *decInfo)
{
    char data = 0;
    //clearing lsb bit of each bytes and storig each bit to the data
    for(int i = 0;i < 8;i++)
    {
	printf("data => %d\n",data);
	data = (image_buffer[i] & 0X1) | (((data) << 1));

	printf("Data in byte from lsb => %c\n",data);
    }
    decInfo -> secret_data = data;
    printf("1 byte decoded data form lsb ==> %c\n",decInfo -> secret_data);
    return e_success;
}

// Encode file extention size
Status decode_size(int size, DecodeInfo *decInfo)
{

    printf("\n\n\nExtr size %d\n",decInfo -> ext_size);
    //getting 32 bytes from source image
    fread(decInfo -> image_data,(size * 8),1,decInfo -> fptr_stego_image);
    //encoding intiger value of size into 32 bytes
    decode_size_from_lsb((size * 8),decInfo -> image_data,decInfo);
    //   decInfo -> ext_size = decInfor -> 32_bit_size;
    // printf("\n\n\nExtr size %d\n",decInfo -> ext_size);
    return e_success;


}

//decode a extention size from LSB of image
Status decode_size_from_lsb(int n_bytes, char *image_buffer,DecodeInfo *decInfo)
{
    decInfo -> _32_bit_size = 0;
    //decoding the lsb bits from each bytes
    for(int i = 0;i < n_bytes;i++)
	decInfo -> _32_bit_size = (image_buffer[i] & 0X1) | ((decInfo -> _32_bit_size) << 1);
    return e_success;
}

// decode secret file extenstion *
Status decode_secret_file_extn(DecodeInfo *decInfo)
{

    //decoding data fro image
    if (decode_data_from_image(decInfo -> ext_size,decInfo -> extn_secret_file,decInfo) == e_success)
    {
	//decode_data_from_image(strlen(MAGIC_STRING),decInfo -> magic_string,decInfo);
	decInfo -> extn_secret_file[decInfo -> ext_size] = '\0';
	printf("extention .txt ==> %s\n",decInfo -> extn_secret_file);
	return e_success;
    }
    else
	return e_failure;
}



// Encode secret file size 
Status decode_file_size(int size, DecodeInfo *decInfo)
{
    //getting 32 bytes from source image
    fread(decInfo -> image_data,(size * 8),1,decInfo -> fptr_stego_image);
    //encoding intiger value of size into 32 bytes
    decode_size_from_lsb((size * 8),decInfo -> image_data,decInfo);
    /*    decode_size_from_lsb((size * 8),decInfo -> image_data,decInfo);
    //encoding the size
    if (encode_size(file_size, encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
    return e_success;
    else
    return e_failure;*/
    return e_success;
}

Status open_secret(DecodeInfo *decInfo)
{
    strcpy(decInfo -> secret_fname,"Secret_decoded");
    strcat(decInfo -> secret_fname,decInfo -> extn_secret_file);
    decInfo -> fptr_secret = fopen(decInfo -> secret_fname,"w");

}


// Encode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char data;
    for (int i = 0;i < (decInfo -> file_size - 1);i++)
    {
	//reading the image data
	fread(decInfo -> image_data,8,1,decInfo -> fptr_stego_image);
	//encoding to the lsb bits
	decode_byte_from_lsb(decInfo -> image_data, decInfo);
	//writing the encoded data into stego image
	fwrite(&(decInfo -> secret_data),1,1,decInfo -> fptr_secret);
    }
    return e_success;

}












