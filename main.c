#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#define CHUNK 50;


void arrayPrint(int *);


void arrayPrint(int * data){
    int * end = data + 4;
    int total;

    for (int * aux = data; aux < end; ++aux) {
        total = total + *aux;
    }
    float per1 = (*data/(float)total) * 100;
    float per2 = (*(data+1)/(float)total) * 100;
    float per3 = (*(data+2)/(float)total) * 100;
    float per4 = (*(data+3)/(float)total) * 100;


    printf("Urna        Numero de Archivos       Histograma\n");
    printf("0-1023      ");
    printf("%d\t                     ", *data);
    for (int i = 0; i <(int)per1; i++){
        printf("%c",'*');
    }
    printf("\n");

    printf("1024-2047   ");
    printf("%d\t                     ", *(data+1));
    for (int i = 0; i <(int)per2; i++){
        printf("%c",'*');
    }
    printf("\n");

    printf("2048-3071   ");
    printf("%d\t                     ", *(data+2));
    for (int i = 0; i <(int)per3; i++){
        printf("%c",'*');
    }
    printf("\n");

    printf("3072-4095   ");
    printf("%d\t                     ", *(data+3));
    for (int i = 0; i <(int)per4; i++){
        printf("%c",'*');
    }

}

int isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

size_t getFileSize(const char* filename) {
    struct stat file_stat;
    stat(filename, &file_stat);
    return file_stat.st_size;
}

char *getln()
{
    char *line = NULL, *tmp = NULL;
    size_t size = 0, index = 0;
    int ch = EOF;

    while (ch) {
        ch = getc(stdin);

        /* Check if we need to stop. */
        if (ch == EOF || ch == '\n')
            ch = 0;

        /* Check if we need to expand. */
        if (size <= index) {
            size += CHUNK;
            tmp = realloc(line, size);
            if (!tmp) {
                free(line);
                line = NULL;
                break;
            }
            line = tmp;
        }

        /* Actually store the thing. */
        line[index++] = ch;
    }

    return line;
}

int * getFileSizeFromDir(DIR * dir, int* buckets, char* dirname) {
    struct dirent * entry;

    while ( (entry = readdir(dir)) != NULL) {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0 )) {
            continue;
        }
        char * newPath = (char*) malloc(2*sizeof(dirname));
        strcpy(newPath, dirname);
        strcat(newPath, "/");
        strcat(newPath, entry->d_name);


        if(isDirectory(newPath)){
            DIR * newDir = opendir(newPath);
            buckets = getFileSizeFromDir(newDir, buckets, newPath);
            continue;
        }



        size_t res = getFileSize(newPath);
        free(newPath);
        if(res < 1024) {
            *(buckets) += 1;
        } else if (res < 2048) {
            *(buckets + 1) += 1;
        } else if (res < 3072) {
            *(buckets +2) += 1;
        } else {
            *(buckets + 3) += 1;
        }

    }
    closedir(dir);

    return buckets;
}


int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int main() {
    DIR *dir;
    struct dirent * entry;
    printf("Please enter the path to the directory you want to traverse: \n");
    char * dirname = getln();
    int * buckets = (int *) malloc(4 * sizeof(int));
    if((dir = opendir(dirname)) != NULL) {
        buckets = getFileSizeFromDir(dir, buckets, dirname);
    }
    arrayPrint(buckets);
    free(buckets);
    free(dirname);
    return 0;
}
