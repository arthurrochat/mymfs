#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* folderPath;
FILE *fs, *fs2;

int checkFile(char* filePath) {
    if ((fs = fopen(filePath, "r")) == NULL){ 
        return 0;
    }
    fclose(fs);
    return 1;
}

int checkConfig() {
    char* filePath = (char *) malloc(strlen(folderPath) + 50);
    strcpy(filePath, folderPath);
    strcat(filePath, "/mymfs.config.txt");
    
    if(checkFile(filePath) == 0) {
        printf("Config not found\n");
        return 0;
    }
    return 1;
}

void copyFile(char* src, char* dest) {
    char ch;

    if(checkConfig() == 1) {
        if(checkFile(dest) == 0) {
            if ((fs = fopen(src, "r")) != NULL){
                fs2 = fopen(dest, "w");
                while((ch = fgetc(fs)) != EOF) {
                    fputc(ch, fs2);
                }
                printf("File created\n");
                fclose(fs);
                fclose(fs2);
            } else {
                printf("Source file not found\n");
            }
        } else {
            printf("Destination file already exists\n");
        }
    }
}

void handleConfig() {
    char* filePath = (char *) malloc(strlen(folderPath));
    strcpy(filePath, folderPath);
    strcat(filePath, "/mymfs.config.txt");
    
    if(checkConfig() == 0){ 
        fs = fopen(filePath, "w");
        fclose(fs);
        printf("Config created\n");
    }
}

void handleImport(char* src) {
    char* dest = (char *) malloc(strlen(folderPath));
    strcpy(dest, folderPath);
    strcat(dest, "/");
    strcat(dest, src);
    copyFile(src, dest);
}

void handleListAll() {
    struct dirent *entry;
    DIR *dir = opendir(folderPath);

    if(dir == NULL) {
        printf("Directory not found\n");
    } else {
        while((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
        closedir(dir);
    }
}

void handleExport(char* src, char* dest) {
    char* fileSrc = (char *) malloc(strlen(folderPath) + strlen(src));
    strcpy(fileSrc, folderPath);
    strcat(fileSrc, src);
    copyFile(fileSrc, dest);
}

void handleParameters(int numArgs, char* args[]) {
    if(numArgs < 3) {
        printf("Missing arguments");
    } else {
        folderPath = args[1];
        if(strcmp(args[2], "config") == 0) {
            handleConfig();
        }
        else if(strcmp(args[2], "import") == 0) {
            if(numArgs == 4) {
                handleImport(args[3]);
            } else {
                printf("Missing arguments\n");
            }
        }
        else if(strcmp(args[2], "listall") == 0) {
            handleListAll();
        }
        else if(strcmp(args[2], "export") == 0) {
            if(numArgs == 5) {
                handleExport(args[3], args[4]);
            } else {
                printf("Missing arguments\n");
            }
        }
        
    }
}

int main(int argc, char* argv[]) {
    handleParameters(argc, argv);
}