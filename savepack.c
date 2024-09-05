
#include <windows.h>
#include <fileapi.h>
#include <stdio.h>

#define FILENAME "SaveData.sav"
#define FSIZE (256*1024)

int main(int argc, char** argv) {
    if (argc != 2) {
        MessageBoxA(NULL, "Drag'N'Drop a extracted folder on me, please", "Error!", MB_ICONEXCLAMATION);
        return 1;
    }

    char abspath[MAX_PATH + 1];
    GetFullPathNameA(argv[1], MAX_PATH+1, abspath, NULL);
    unsigned int dir_glob_length = strlen(abspath) + 2 + 1;
    char dir_glob[dir_glob_length];
    memcpy(dir_glob, abspath, dir_glob_length - 3);
    dir_glob[strlen(abspath)] = '\\';
    dir_glob[strlen(abspath)+1] = '*';
    dir_glob[dir_glob_length] = 0;

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(dir_glob, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) {
        MessageBoxA(NULL, "Error listing files. Does this folder exist?", "Error!", MB_ICONEXCLAMATION);
        return 1;
    }

    FILE* f = fopen(FILENAME, "w");
    char a = 1;
    fwrite(&a, 1, 1, f);

    int abspath_length = strlen(abspath);

    do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            int namesize = strlen(ffd.cFileName);
            char full_path[abspath_length + 1 + namesize + 1];
            full_path[abspath_length + 1 + namesize] = 0;
            memcpy(full_path, abspath, abspath_length);
            full_path[abspath_length] = '\\';
            memcpy(full_path + abspath_length + 1, ffd.cFileName, abspath_length);

            int filesize = ffd.nFileSizeLow;
            char file[filesize+1];
            file[filesize] = 0;

            FILE* fi = fopen(full_path, "r");
            fread(file, 1, filesize, fi);
            fclose(fi);

            fwrite(&namesize, sizeof(int), 1, f);
            fwrite(ffd.cFileName, 1, namesize, f);
            fwrite(&filesize, sizeof(int), 1, f);
            fwrite(file, 1, filesize, f);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    int deadbeef = 0xDEADBEEF;
    fwrite(&deadbeef, sizeof(int), 1, f);

    if (ftell(f) < FSIZE) {
        int zeros = FSIZE - ftell(f);
        char zero = 0;
        for (int i = 0; i < zeros; i++) fwrite(&zero, 1, 1, f);
    }

    FindClose(hFind);
    fclose(f);
    return 0;
}
