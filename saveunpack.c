
#include <fileapi.h>
#include <windows.h>
#include <stdio.h>

#define DIRECTORY "extracted_savefile"

int main(int argc, char** argv) {
    if (argc != 2) {
        MessageBoxA(NULL, "Drag'N'Drop a SaveData.sav on me, please", "Error!", MB_ICONEXCLAMATION);
        return 1;
    }
    CreateDirectoryA(DIRECTORY, NULL);
    FILE* f = fopen(argv[1], "r");
    fseek(f, 1, SEEK_SET);

    while (1) {
        unsigned int name_length;
        fread(&name_length, sizeof(int), 1, f);
        if (name_length == 0xDEADBEEF) break;

        char* name = malloc(name_length+1);
        name[name_length] = 0;
        fread(name, sizeof(char), name_length, f);

        unsigned int content_length;
        fread(&content_length, sizeof(int), 1, f);

        char* content = malloc(content_length+1);
        content[content_length] = 0;
        fread(content, sizeof(char), content_length, f);
        
        char* dir_name = DIRECTORY;
        int dir_length = strlen(dir_name);
        char filename[dir_length + 1 + name_length + 1];
        memcpy(filename, dir_name, dir_length);
        filename[dir_length] = '/';
        memcpy(filename + dir_length + 1, name, name_length);
        filename[dir_length + 1 + name_length] = 0;

        FILE* file = fopen(filename, "w");
        fwrite(content, sizeof(char), content_length, file);
        fclose(file);

        printf("%s: %u bytes\n", name, content_length);
    }

    fclose(f);
    return 0;
}
