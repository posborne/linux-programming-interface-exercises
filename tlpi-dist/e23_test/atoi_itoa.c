
//#include <pthread.h>
#include "tlpi_hdr.h"

//prototpe, should be move to headfile later.
int my_atoi(const char* str);
int my_itoa(const int input);
void print_usage(void);

//convert string to integer.
int my_atoi(const char* str){

 if(!str) return -EINVAL;

 const char* tmp_ptr=str;
 int   result=0;
 int   sign =0;  //0/1: positive/negative
 if((*tmp_ptr) == '-'){
     sign =1;
     ++tmp_ptr;
 }
 while ((*tmp_ptr) != '\0'){

     //todo , check the *temp_ptr character here shoud be '0'~'9'
     if((*tmp_ptr) < '0' || (*tmp_ptr) > '9') return -EINVAL;

     result *= 10;
     result +=(*tmp_ptr)-'0';	     
     ++tmp_ptr;
 }
 if(sign){
    result = -result;
 }
 printf ("my_atoi input=%s, output=%d\n",str,result);

 return 0;
}

//convet integer to string.
int my_itoa(const int input){
 
 int iRet=0;
 int sign=0;  //0/1 : positive/netagive
 int tmp_input = input;
 if  (tmp_input < 0){
 	sign = 1;
	tmp_input = - tmp_input;
 }

 //get input length
 int input_len=1;
 while (tmp_input!=0 && (tmp_input/=10) != 0){
     ++input_len;
 }
 printf ("intput number=%d, length=%d\n",input,input_len);
 
 //determine the string length
 int str_len=0;
 if (sign == 1){
     str_len = 1+input_len+1;  //sign + number + '\0'
     tmp_input= -input;
 }
 else{
    str_len= input_len+1;     //number + '\0'
    tmp_input=  input;
 }

 printf ("str_len=%d\n",str_len);

 //allocate memory.
 char* result_str=NULL;
 result_str = (char*) calloc(str_len,sizeof(char));
 if (result_str == NULL){
    errExit("calloc"); 
 }
 
 /*point to the final char and assign '\0' character. */
 int copy_index=(str_len-1);     
 result_str[copy_index--]='\0'; 

 //This is special case
 if(tmp_input ==0){
     result_str[copy_index] = '0';
 }
 else{
   while(tmp_input != 0){
     result_str[copy_index--] = (tmp_input % 10) +'0';
     tmp_input /=10;
   }
 }


 //assign - character 
 if (sign == 1){
    result_str[copy_index] = '-';
 }

 printf("result_str=%s\n",result_str);
 if (result_str){
    free(result_str);
 }

 return iRet;

}


void print_usage(void){
  printf("--atoi [str]\n");
  printf("--itoa [int]\n");
}

int
main(int argc, char *argv[])
{
    int iRet=0;
    if(argc <= 2){
      print_usage(); 
    }
    else{
      if( !strcmp(argv[1],"--atoi") ){
        const char* input_str=argv[2];
	if( (iRet=my_atoi(input_str)) < 0 ){
	   printf("invalid argument\n");
	}
      }
      else if( !strcmp(argv[1],"--itoa")){
        const int input_num= atoi(argv[2]);
	my_itoa(input_num);
      }
      else{
       print_usage();
      }

    }
    //errExitEN(iRet, "pthread_join");
    //errExit("calloc");


    exit(EXIT_SUCCESS);
}
