#include <stdlib.h>
#include <stdio.h>
#include "string.h"


typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    int displayMode;
} state;

typedef struct {
    char* name;
    void (*fun)(state*);
}funDesc;

int isDebug(state* s){
    return s->debug_mode == '1';
}

void toggleDebugMode(state* s){
    if(isDebug(s)){
        s->debug_mode = '0';
        printf("Debug flag now off\n");
    }else{
        s->debug_mode = '1';
        printf("Debug flag now on\n");
    }
}

void quit(state* s){
    if(isDebug(s)){
        printf("QUITING\n");
    }
    exit(0);
}

void setFileName(state* s){
    printf("Please Enter File Name\n");
    char fileName[128];
    if(fgets(fileName, sizeof(fileName), stdin) != NULL)
        memcpy(s->file_name, strtok(fileName, "\n"), sizeof (fileName));

    if(isDebug(s))
        printf("DEBUG: file name set to %s\n", s->file_name);
}

int isValidUnitSize(int unitSize){
   return (unitSize == 1 || unitSize == 2 || unitSize == 4);
}

void setUnitSize(state* s){
    printf("Please Enter Unit Size\n");
    char unitSizeStr[10];
    int unitSize;
    if(fgets(unitSizeStr, sizeof(unitSizeStr), stdin) != NULL) {
        sscanf(unitSizeStr, "%d", &unitSize);
        if (isValidUnitSize(unitSize))
            s->unit_size = unitSize;
        else {
            if (s->displayMode)
                printf("ERROR: %X is not a valid size, please chose from 0x1, 0x2, 0x4\n", unitSize);
            else
                printf("ERROR: %i is not a valid size, please chose from 1, 2, 4\n", unitSize);
        }
    }

    if(isDebug(s)){
        if(s->displayMode)
            printf("DEBUG: unit size set to %X\n", s->unit_size);
        else
            printf("DEBUG: unit size set to %i\n", s->unit_size);
    }
}

void loadToMemory(state* s){
    if(strcmp(s->file_name, "") == 0)
        printf("ERROR: no file name was set, please use option 1 to set file name\n");
    else{
        FILE* f = fopen(s->file_name, "r+");
        if(f == NULL)
            printf("ERROR: file %s does not exist.\n", s->file_name);
        else{
            printf("Please Enter <location> <length>\n");
            char input[10000];
            fgets(input, sizeof(input), stdin);
            int location;
            int length;
            sscanf(input, "%x %i", &location, &length);

            if(isDebug(s)){
                if(s->displayMode)
                    printf("File Name: %s Location: %X Length: %X\n", s->file_name, location, length);
                else
                    printf("File Name: %s Location: %X Length: %i\n", s->file_name, location, length);
            }

            fseek(f, location, SEEK_SET);
            fread(s->mem_buf, 1, s->unit_size * length ,f);
            if(s->displayMode)
                printf("Loaded %X units into memory\n", length);
            else
                printf("Loaded %i units into memory\n", length);
            fclose(f);
        }
    }
}

void toggleDisplayMode(state* s){
    if(s->displayMode){
        s->displayMode = 0;
        printf("Display flag off now, decimal representation.\n");
    }else{
        s->displayMode = 1;
        printf("Display flag on now, hexadecimal representation.\n");
    }
}

char* getStringFormat(state* s){
    char* decFormat[4] = {"%#hhd\n", "%#hd\n", "", "%#d\n"};
    char* hexFormat[4] = {"%#hhx\n", "%#hx\n", "", "%#x\n"};
    if(s->displayMode)
        return hexFormat[s->unit_size - 1];
    return decFormat[s->unit_size - 1];
}

void memoryDisplay(state* s){
    printf("Please Enter <location> <units>\n");
    char input[10000];
    fgets(input, sizeof(input), stdin);
    int location;
    int units;
    sscanf(input, "%x %i", &location, &units);

    if(isDebug(s)){
        if(s->displayMode)
            printf("Location: %X Length: %X\n", location, units);
        else
            printf("Location: %X Length: %i\n", location, units);
    }

    void* ptr;
    if(location == 0)
        ptr = &(s->mem_buf);
    else
        ptr = &location;
    while(units > 0){
        int var = *((int*)ptr);
        printf(getStringFormat(s), var);
        units--;
        ptr = ptr + s->unit_size;
    }
}

void saveIntoFile(state* s){
    printf("Please Enter <source-address> <target-location> <length>\n");
    char input[10000];
    fgets(input, sizeof(input), stdin);
    int sourceAddress;
    int targetLocation;
    int length;
    sscanf(input, "%x %x %i", &sourceAddress, &targetLocation, &length);

    if (isDebug(s)) {
        if (s->displayMode)
            printf("length = %X, units= %X from memory, starting at address %X to the file %s, starting from offset %X\n",
                   length, s->unit_size, sourceAddress, s->file_name, targetLocation);
        else
            printf("length = %d, units = %d from memory, starting at address %d to the file %s, starting from offset %d\n",
                   length, s->unit_size, sourceAddress, s->file_name, targetLocation);
    }

    void* ptr;
    if(sourceAddress == 0)
        ptr = &(s->mem_buf);
    else
        ptr = &sourceAddress;

    FILE* f = fopen(s->file_name,"r+");
    if(f != NULL){
        fseek(f, targetLocation, SEEK_SET);
        if(ftell(f) == targetLocation) {
            fwrite(ptr, 1, s->unit_size * length, f);
        }
        else
            printf("ERROR: target location: %X is out of bound for file: %s", targetLocation, s->file_name);
        fclose(f);
    }else{
        printf("ERROR:CANT OPEN FILE: %s", s->file_name);
    }
}

void memoryModify(state* s){
    printf("Please Enter <location> <val>\n");
    char input[10000];
    fgets(input, sizeof(input), stdin);
    int location;
    int val;
    sscanf(input, "%x %x", &location, &val);

    if(isDebug(s)){
        if(s->displayMode)
            printf("Location: %X Length: %X\n", location, val);
        else
            printf("Location: %X Length: %i\n", location, val);
    }

    if(location < sizeof(s->mem_buf))
        memcpy(&s->mem_buf[location], &val, s->unit_size);
}



void notImplemented(state* s){
    printf("NOT IMPLEMENTED YET");
}

int main(int argc, char** argv){
    state* myState = calloc(1,sizeof (state));
    funDesc funcs[9] = {{"Toggle Debug Mode", &toggleDebugMode},{"Set File Name", &setFileName}, {"Set Unit Size", &setUnitSize},
                         {"Load Into Memory", &loadToMemory},{"Toggle Display Mode", &toggleDisplayMode},{"Memory Display", &memoryDisplay},
                         {"Save Into File", &saveIntoFile}, {"Memory Modify", &memoryModify},{"Quit", &quit}};
    while(1){
        if(myState->debug_mode == '1') {
            if(myState->displayMode)
                printf("Debug: unit_size: %X file_name: %s mem_count: %X\n", myState->unit_size, myState->file_name, myState->mem_count);
            else
                printf("Debug: unit_size: %d file_name: %s mem_count: %zu\n", myState->unit_size, myState->file_name, myState->mem_count);

        }

        printf("Chose a function number:\n");
        for(int i = 0;  i < 9 ; i++)
            printf("%i- %s\n", i, funcs[i].name);

        char input[2048];
        if(fgets(input,  sizeof (input), stdin) != NULL){
            int index;
            sscanf(input ,"%d", &index);
            funcs[index].fun(myState);
        }


    }
}

