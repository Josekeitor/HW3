#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#define CHUNK 50;

int isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

size_t getFileSize(const char* filename) {
    struct stat file_stat;
    if(stat(filename, &file_stat) != 0) {
        return 0;
    }
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
        char * newPath = (char*) malloc(2*sizeof(dirname));
        strcpy(newPath, dirname);
        strcat(newPath, "/");
        strcat(newPath, entry->d_name);

        if(isDirectory(newPath)){
            if(entry->d_namlen < 3){
                free(newPath);
                continue;
            }
            DIR * newDir = opendir(newPath);
            return getFileSizeFromDir(newDir, buckets, newPath);
        }


        free(newPath);
        size_t res = getFileSize(entry->d_name);

        if(res < 1024) {
            *(buckets) += 1;
        } else if (res < 2048) {
            *(buckets + 1) += 1;
        } else if (res < 3072) {
            *(buckets +2) += 1;
        } else if (res < 4096) {
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
    for(int i = 0; i < 4; i++){
        printf("%d", *(buckets+i));
    }
    free(buckets);
    free(dirname);
    return 0;
}