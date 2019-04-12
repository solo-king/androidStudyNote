#include <stdio.h>
#include <stdlib.h>

int main(int argc , char** argv)
{
	FILE * pfd = NULL;
	char *lineptr = NULL;
	ssize_t line_length = 0;
	size_t read_len = 0;
	
	if(argc != 2 ){
		printf("Usage:%s <shellcmd>\n", argv[0]);
		return -1;
	}
	
	printf("cmd is:%s\n", argv[1]);
	
	pfd = popen(argv[1], "r");
//	pfd = fopen("./test.c", "r");
	if(pfd == NULL){
		printf("pdf is NULL\n");
		return -1;
	}

	while((line_length = getline(&lineptr, &read_len, pfd)) != -1){
		
		printf("lineptr = %s\n", lineptr);
		printf("line_length = %zu\n", line_length);
	//	free(lineptr);
	//	lineptr = NULL;
		
	}

	free(lineptr);
	lineptr = NULL;
	pclose(pfd);
	return 0;
}
