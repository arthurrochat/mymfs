#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* folderPath;
FILE *fs, *fs2;

void addToConfig(char* fileName) {
    char* filePath = (char *) malloc(strlen(folderPath) + 50);
    strcpy(filePath, folderPath);
    strcat(filePath, "/mymfs.config.txt");
    if ((fs = fopen(filePath, "a")) != NULL) {
        fputs(fileName, fs);
        fprintf(fs, "\n");
    }
}

int checkFileInConfig(char* fileName) {
    int found = 0;
    char str[1000];
    char* filePath = (char *) malloc(strlen(folderPath) + 50);
    strcpy(filePath, folderPath);
    strcat(filePath, "/mymfs.config.txt");
    if ((fs = fopen(filePath, "r")) != NULL){
        while((fgets(str, sizeof(str), fs)) != NULL && found == 0) {
            if(strstr(str, fileName) != NULL) {
                found = 1;
            }
        }
    }
    if(found == 0) {
        printf("Cannot find file in config");
    }

    return found;
}

int checkFile(char* filePath) {
    if ((fs = fopen(filePath, "r")) == NULL){ 
        return 0;
    }
    fclose(fs);
    return 1;
}

char* getFilePath(char* fileName) {
    char* filePath = (char *) malloc(strlen(folderPath) + strlen(fileName));
    strcpy(filePath, folderPath);
    strcat(filePath, fileName);
    return filePath;
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
    if(checkConfig() == 1) {
        char* dest = (char *) malloc(strlen(folderPath));
        strcpy(dest, folderPath);
        strcat(dest, "/");
        strcat(dest, src);
        copyFile(src, dest);
        addToConfig(src);
    }
}

void handleListAll() {
    struct dirent *entry;

    if(checkConfig() == 1) {
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
}

void handleExport(char* src, char* dest) {
    if(checkConfig() == 1 && checkFileInConfig(src) == 1) {
        char* fileSrc = (char *) malloc(strlen(folderPath) + strlen(src));
        strcpy(fileSrc, folderPath);
        strcat(fileSrc, src);
        copyFile(fileSrc, dest);
    }
}

void handleRemove(char* fileName) {
    if(checkConfig() == 1 && checkFileInConfig(fileName) == 1) {
        char* filePath = (char *) malloc(strlen(folderPath) + strlen(fileName));
        strcpy(filePath, folderPath);
        strcat(filePath, "/");
        strcat(filePath, fileName);
        int status = remove(filePath);
        if(status == 0) {
            printf("File deleted successfully");
        } else {
            printf("Cannot delete file");
        }
    }
}

void handleRemoveAll() {
    if(checkConfig() == 1) {
        char str[500];
        char* filePath = (char *) malloc(strlen(folderPath) + 50);
        strcpy(filePath, folderPath);
        strcat(filePath, "/mymfs.config.txt");
        if ((fs2 = fopen(filePath, "r")) != NULL){
            while((fgets(str, sizeof(str), fs)) != NULL) {
                handleRemove(strtok(str, "\n"));
            }
        }
    }
}

void handleHead100(char* fileName) {
    if(checkConfig() == 1 && checkFileInConfig(fileName) == 1) {
        int counter = 0;
        char ch;
        char* filePath = getFilePath(fileName);
        if ((fs = fopen(filePath, "r")) != NULL){
            while((ch = fgetc(fs)) != EOF || counter == 100) {
                printf("%c", ch);
                counter++;
            }
        } else {
            printf("Source file not found\n");
        }
    }
}

void handleTail100(char* fileName) {
    if(checkConfig() == 1 && checkFileInConfig(fileName) == 1) {
        int counter = 0;
        int size = 5;
        char ch;
        char content[size];
        char* filePath = getFilePath(fileName);

        if ((fs = fopen(filePath, "r")) != NULL){
            while((ch = fgetc(fs)) != EOF) {
                if(counter >= size) {
                    content[counter - (size*(counter/size))] = ch;
                } else {
                    content[counter] = ch;
                }
                counter++;
            }
        } else {
            printf("Source file not found\n");
        }
        int i;
        int rep = (counter < size) ? counter : size;
        for(i = 0; i < rep; i++) {
            printf("%c", content[i]);
        }
    }
}

void handleGrep(char* fileName, char* word) {
    char* filePath = getFilePath(fileName);
    char str[500];
    char* result;
    int counter = 0;
    int found = 0;

    if(checkConfig() == 1 && checkFileInConfig(fileName) == 1) {
        if ((fs = fopen(filePath, "r")) != NULL) {
            while((fgets(str, sizeof(str), fs)) != NULL && found == 0) {
                counter++;
                result = strstr(str, word);
                if(result != NULL) {
                    found = 1;
                }
            }
            if(found == 1) {
                printf("Found in line %d", counter);
            } else {
                printf("Not found");
            }
        } else {
            printf("Source file not found\n");
        }
    }
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
        else if(strcmp(args[2], "remove") == 0) {
            if(numArgs == 4) {
                handleRemove(args[3]);
            } else {
                printf("Missing arguments\n");
            }
        }
        else if(strcmp(args[2], "removeall") == 0) {
            handleRemoveAll();
        }
        else if(strcmp(args[2], "head100") == 0) {
            if(numArgs == 4) {
                handleHead100(args[3]);
            } else {
                printf("Missing arguments\n");
            }
        }
        else if(strcmp(args[2], "tail100") == 0) {
            if(numArgs == 4) {
                handleTail100(args[3]);
            } else {
                printf("Missing arguments\n");
            }
        }
        else if(strcmp(args[2], "grep") == 0) {
            if(numArgs == 5) {
                handleGrep(args[4], args[3]);
            } else {
                printf("Missing arguments\n");
            }
        }
        
    }
}

int main(int argc, char* argv[]) {
    handleParameters(argc, argv);
}