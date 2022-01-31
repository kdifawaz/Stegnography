/*
PROJECT : Steganography

Name         :FAWAZ
Date         :31-01-2022
Description  :Encode a message such a way that the quality of the image doesn't change much and decoding.
Sample input :
		./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]
		./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]
Sample output:
		user@user:~] ./lsb_steg -e SkeletonCode/beautiful.bmp secret.txt
		INFO: Opening required files
		INFO: Opened SkeletonCode/beautiful.bmp
		INFO: Opened secret.txt
		INFO: Opened steged_img.bmp
		INFO: Done
		INFO: ## Encoding Procedure Started ##
		INFO: Checking for secret.txt size
		INFO: Done. Not Empty
		INFO: Checking for SkeletonCode/beautiful.bmp capacity to handle secret.txt
		INFO: Done. Found OK
		INFO: Output File not mentioned. Creating steged_img.bmp as default
		INFO: Copying Image Header
		INFO: Done
		INFO: Encoding Magic String Signature
		INFO: Done
		INFO: Encoding secret.txt File Extenstion
		INFO: Done
		INFO: Encoding secret.txt File Size
		INFO: Done
		INFO: Encoding secret.txt File Data
		INFO: Done
		INFO: Copying Left Over Data
		INFO: Done
		INFO: ## Encoding Done Successfully ##
*/





/*Addign header files*/

#include <stdio.h>
#include "decode.h"
#include "encode.h"
#include "types.h"

/*Main funciton*/
int main(int argc,char *argv[])
{
    /*Declaring two structure variables*/
    DecodeInfo decInfo;
    EncodeInfo encInfo;

    int type,status;
    // Checking the opration type
    type = check_operation_type(argv);
    if ( type == e_encode)
    {
	printf("## Encoding Proccedure Started ##\n");
	/*validating the arguments before starting encoding*/
	if (read_and_validate_encode_args(argv,&encInfo) == e_success)
	    do_encoding(&encInfo);
	else
	    printf("Error:Invalid input\nUsage:./lsb_steg -e <file.bmp> <secret.txt> [output file]\n");

    }
    else if(type == e_decode) 
    {
	printf("## Decoding Proccedure Started ##\n");
	/*validating the arguments before starting decoding*/
	if (read_and_validate_decode_args(argv,&decInfo) == e_success)
	    do_decoding(&decInfo);
	else
	    printf("Error:Invalid input\nUsage:./lsb_steg -e <file.bmp> <secret.txt> [output file]\n");
    }
    else
	    printf("Error:Invalid input\nUsage:./lsb_steg -e <file.bmp> <secret.txt> [output file]\n");



    return 0;
}
