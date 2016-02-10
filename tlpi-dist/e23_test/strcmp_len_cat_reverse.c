
//#include <pthread.h>
#include "tlpi_hdr.h"

//prototpe, should be move to headfile if necessary.
void print_usage(void);
int my_strcmp (const char* str1, const char* str2);
int my_strcat (char* str1, const char* str2);
int my_strlen (const char* str1);
int my_strreverse (char* str1);

/*
return = 1  if str1 >  str2
         0  if str1 == str2
	 -1 if str1 <  str2
*/
int my_strcmp (const char* str1, const char* str2){
 
  if(!str1 || !str2){
      return EINVAL;
  }

  printf ("input str1=%s\n",str1);
  printf ("input str2=%s\n",str2);

  while (*str1 != '\0'){  // '\0'== 0     
     if (*str2 == '\0'){
         return 1;
     }
     if(*str1 > *str2){
	    return 1;
     }
     if(*str1 < *str2){
           return -1;
     }
     ++str1;
     ++str2;
  }
  if (*str2 == '\0') return 0;
  else return 1;
}


int my_strcat (char* str1, const char* str2){
 
  if(!str1 || !str2){
      return EINVAL;
  }

  printf ("input str1=%s\n",str1);
  printf ("input str2=%s\n",str2);

  //find the '\0' character of str1
  char* tmp_ptr=str1;
  while(*tmp_ptr){
    ++tmp_ptr;
  }
  
  //copy character from str2 to str1
  while(*str2){
    (*tmp_ptr)=(*str2);
    tmp_ptr++;
    str2++;
  }
  *(++tmp_ptr)='\0';

 printf("str=%s\n",str1);
 return 0;

}

int my_strlen (const char* str1){

  if(!str1){
    return EINVAL;
  }

  size_t str_len=0;
  while(*str1){
    ++str_len;
    ++str1;
  }
  printf ("input str=%s,length=%d\n",str1,str_len);
  return 0;

}

int my_strreverse (char* str1){

  if(!str1){
    return EINVAL;
  }

  printf ("input str=%s\n",str1);

  char* str_start=str1;
  char* str_end=str1;

  while(*str_end){
    ++str_end;
  }
  --str_end;

  while(str_start < str_end){
    
    char tmp_char=(*str_start);
    (*str_start)=(*str_end);
    (*str_end)=tmp_char;
    ++str_start;
    --str_end;
  }
  printf ("outputstr=%s\n",str1);
  return 0;

}


void print_usage(void){
  printf("--strcmp [str1] [str2]\n");
  printf("--strcat [str1] [str2]\n");
  printf("--strlen [str1]\n");
  printf("--strreverse [str1]\n");
}


int
main(int argc, char *argv[])
{
    //int iRet=0;
    
    if(argc <= 2){
      print_usage(); 
    }
    else{
      if( !strcmp(argv[1],"--strcmp") ){
        const char* str1=argv[2];
	const char* str2=argv[3];
        if (!str1 || !str2){
           printf("invalid argument");
	}  
	int result=my_strcmp(str1,str2);
	printf ("result=%d\n",result);
      }
      else if( !strcmp(argv[1],"--strcat") ){
        const char* str1=argv[2];
        const char* str2=argv[3];
        if (!str1 || !str2){
           printf("invalid argument");
	}  
	else{
   	   char* str_buffer=(char*)calloc(strlen(str1)+strlen(str2)+1,sizeof(char));
	   memcpy(str_buffer,str1,sizeof(str1));
	   my_strcat(str_buffer,str2);
	   free(str_buffer);
	}
      }
      else if( !strcmp(argv[1],"--strlen") ){
	const char* str1=argv[2];
	if(!str1){
           printf("invalid argument");
	}
        my_strlen(str1);
      }
      else if( !strcmp(argv[1],"--strreverse") ){
	char* str1=argv[2];
	if(!str1){
           printf("invalid argument");
	}
        my_strreverse(str1);
      }
      else{
       print_usage();
      }

    }
    
    //errExitEN(iRet, "pthread_join");
    //errExit("calloc");


    exit(EXIT_SUCCESS);
}
