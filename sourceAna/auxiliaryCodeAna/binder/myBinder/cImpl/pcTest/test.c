#define _GNU_SOURCE
       #include <stdio.h>
       #include <stdlib.h>

       int
       main(void)
       {
           FILE *stream;
           char *line = NULL;
           size_t len = 0;
           ssize_t read;

           stream = fopen("./test.c", "r");
           if (stream == NULL)
               exit(EXIT_FAILURE);

           while ((read = getline(&line, &len, stream)) != -1) {
               printf("Retrieved line of length %zu :\n", read);
               printf("%s", line);
           }

           free(line);
           fclose(stream);
           exit(EXIT_SUCCESS);
       }

