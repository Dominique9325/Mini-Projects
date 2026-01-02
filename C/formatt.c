#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
 
#define ERR_INV_PAR(off) "Error: '" off "' is not a valid parameter.\n"
#define ERR_DUP_PAR "Error: Duplicate parameters.\n"
#define ERR_INC_PAR(off1,off2) "Error: Parameters '" off1"' and '" off2 "' are not compatible.\n"
 
typedef struct
{
    unsigned char flgInvParam;                      // flag for an invalid parameter, as soon as one is detected this flag is tripped and the entire program stops.
    char invParam;                                  // the invalid parameter in question
    unsigned char flgDupParam;                      // flag for duplicate parameters, using the same parameter twice is not allowed.
    unsigned char flgIncompParam;                   // flag for incompatible parameters, so far there are only 2 mutually incompatible parameters (f and h), of which one is not implemented yet (h).
    char incompParam1, incompParam2;                // the incompatible parameters in question
    char** strsArr;
}PCTRL;                                             // control structure that is supposed to hold the raw argument string array (no parameters), as well as a bunch of flags for determining the validity of parameters, a bit redundant and weird i did it in this way but nvm. i probably should've
                                                    // put the parameters string inside it.
 
typedef struct
{
    size_t rawCnt;                                  // number of raw strings in the array
    char** strsRaw;                                 // raw string array
}RAWPKG;                                            //a slightly less redundant structure, used to hold non-paramter arguments as strings in the string array.
 
unsigned char finParamValidation(PCTRL pControlStruct, unsigned char flgParams)             // final parameter validation. checks for invalid parameters, duplicate parameters and incompatible parameters, in case any are detected the text "formatting" is aborted
{                                                                                           // and the program ends. it prints the corresponding error and the offending parameters that caused it if an error does occur.
    if (pControlStruct.flgInvParam & flgParams)
        fprintf(stderr, ERR_INV_PAR("%c"), pControlStruct.invParam);
    if (pControlStruct.flgDupParam & flgParams)
        fprintf(stderr, ERR_DUP_PAR);
    if (pControlStruct.flgIncompParam & flgParams)
        fprintf(stderr, ERR_INC_PAR("%c","%c"), pControlStruct.incompParam1, pControlStruct.incompParam2);
    return !((pControlStruct.flgInvParam | pControlStruct.flgIncompParam | pControlStruct.flgDupParam) & flgParams);
}
 
void readFromFile(char** str)                                                               // replaces the string (path to file) with the content of the file at its location, only for textual-type files.
{
    if (!(*str))
        return;
    FILE* fp = fopen(*str, "r");
    if (!fp)
    {
        fprintf(stderr,"Error: The file \"%s\" could not be found. The argument has not been expanded.\n", *str);
        return;
    }
    char buffer[1024];
    char c;
    size_t i = 0;
    while (1)
    {
        c = fgetc(fp);
        if (c == EOF)
            break;
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    fclose(fp);
    free(*str);
    char* temp;
    *str = (temp = strdup(buffer)) ? temp : NULL;
}
 
unsigned char isInS(char c, char* str) // for checking if a certain character is inside a string
{
    if (!str)
        return 0;
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == c)
            return 1;
    }
    return 0;
}
 
unsigned char isInST(size_t e, size_t* arr, size_t count) // for checking if a certain size_t element is inside an array of size_t elements, i only use this function once.
{
    if (!arr)
        return 0;
    for (size_t i = 0; i < count; i++)
    {
        if (arr[i] == e)
            return 1;
    }
    return 0;
}
 
void printStrs(RAWPKG package,char* params) // for printing/directing the processed strings/arguments to stdout.
{
    if (!package.strsRaw)
    {
        fprintf(stderr,"Error: NULL string array.\n"); // just in case
        return;
    }
    unsigned char hasFiles = params ? ((params && isInS('f',params)) ? 1 : 0) : 0; // flag for checking if the f attribute/parameter is active, for formatting purposes if it is active the printing process slightly differs since newlines are unnecessary
    for (size_t i = 0; i < package.rawCnt; i++)
    {
        if (!package.strsRaw[i])
        {
            fprintf(stderr,"Error: NULL string at index %lu.\n",package.rawCnt);
            return; 
        }
        printf("%s", package.strsRaw[i]);
        if (!hasFiles)
            printf("\n");
    }
}
 
unsigned char chkParams(int argc, char** argv, unsigned char* flgParams, char** params) // the first parameter/attribute check function. checks if they are present in argv and accordingly sets the parameters flag, and extracts the parameters into the string
{                                                                                       // aborts the program if there are no non-parameter arguments or if parameters are announced via a hyphen but not specified.
    if (argc == 1 || (argc == 2 && argv[1][0] == '-'))
    {
        fprintf(stderr,"Error: Invalid numbers of arguments(0)\n");
        return 0;
    }
    else if (argv[1][0] == '-' && strlen(argv[1]) == 1)
    {
        fprintf(stderr,"Error: Parameters announced, but not specified.\n");
        return 0;
    }
    *flgParams = argv[1][0] == '-' ? 1 : 0;
    *params = *flgParams ? strdup(argv[1]) : NULL;
    if (flgParams && !params)
        return 0;
    return 1;
}
 
RAWPKG xtrRawStrs(int argc, char** argv, unsigned char flgParams)            // function for extracting non-attribute arguments into a string array which is packed up in a RAWPKG struct along with the count of non-attribute arguments.
{                                                                            // if the flgParams flag is negative (meaning there are no attributes), the rawCnt non-attribute argument count isn't decreased.
    RAWPKG package;
    package.strsRaw = (char**)malloc((argc-1-flgParams)*sizeof(char*));      // note that the first argument in argv is actually the name of the command, therefore it's ignored.
    if (!package.strsRaw)
    {
        package.rawCnt = 0;
        return package;
    }
    package.rawCnt = argc-1-(size_t)flgParams;
        size_t j = 0;
        for (size_t i = 1+flgParams; i < argc; i++)
        {
            package.strsRaw[j++] = strdup(argv[i]);
        }
    return package;
}
 
void strrev(char* str)                                                        // my version of strrev, since i wrote this in vi editor in a linux mint vm, using GCC to compile the program, strrev apparently isn't part of the C standard library, 
{                                                                             // so i wrote my own. unlike the original the reversal is done in place.
    if (!str)
        return;
    char* tempStr = (char*)malloc((strlen(str)+1)*sizeof(char));
    if (!tempStr)
        return;
    size_t j = strlen(str)-1;
    for (size_t i = 0; str[i] != '\0'; i++)
        tempStr[i] = str[j--];
    strcpy(str,tempStr);
    if (tempStr)
        free(tempStr);
}
 
unsigned char noRepeat(char* str)                                             // checks for repeating arguments, returns 1 (true) if there are none.
{
    if (!str)
        return 0;
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        for (size_t j = 0; str[j] != '\0'; j++)
        {
            if (str[i] == str[j] && i!=j)
                return 0;
        }
    }
    return 1;
}
 
void relStrArrMem(char*** strArr, size_t elNum)                              // deallocates the memory of all the strings within the raw string array, then deallocates the memory of the string array itself and sets all the pointer to NULL
{
    if (!(*strArr))
        return;
    for (size_t i = 0; i < elNum; i++)
    {
        if ((*strArr)[i])
        {
            free((*strArr)[i]);
            (*strArr)[i] = NULL;
        }
    }
    free((*strArr));
    *strArr = NULL;
}
 
int cmp(const void* elA, const void* elB)                                   // my compare function for the -s sort attribute, since it uses qsort.
{
    return *((char*)elA) > *((char*)elB);
}
 
void fill(void** arr, size_t count, void* el, char* type)                   // a very disgusting function, probably was unnecessary to make it so long. fills an array with a specified element, compatible with multiple types, the type is passed as an argument.
{                                                                           // used it only once.
    if (!arr)
        return;
    for (size_t i = 0; i < count; i++)
    {
        if (!strcmp(type,"char"))
            (*((char**)arr))[i] = *((char*)el);
        else if (!strcmp(type,"int"))
            (*((int**)arr))[i] = *((int*)el);
        else if (!strcmp(type,"double"))
            (*((double**)arr))[i] = *((double*)el);
        else if (!strcmp(type,"float"))
            (*((float**)arr))[i] = *((float*)el);
        else if (!strcmp(type,"size_t"))
            (*((size_t**)arr))[i] = *((size_t*)el);
        else if (!strcmp(type,"short"))
            (*((short**)arr))[i] = *((short*)el);
        else if (!strcmp(type,"long"))
            (*((long**)arr))[i] = *((long*)el);
    }
}
 
void scramble(char* str)                                                // an unnecessarily complicated string scramble function. it uses a temporary string and an array of shuffled indices to construct the new scrambled string.
{
    if (!str)
        return;
    size_t i;
    char* tempStr = (char*)malloc((strlen(str)+1)*sizeof(char));
    if (!tempStr)
        return;
    size_t scrIndex;
    size_t filler = strlen(str)+2;
    size_t* occupied = (size_t*)malloc(strlen(str)*sizeof(size_t));
    if (!occupied)
        return;
    fill((void**)(&occupied),strlen(str),(void*)(&filler),"size_t");
    for (i = 0; i < strlen(str); i++)
    {
        do {scrIndex = rand() % (strlen(str));}
        while (isInST(scrIndex, occupied, strlen(str)));
        occupied[i] = scrIndex;
        tempStr[i] = str[scrIndex];
    }
    tempStr[i] = '\0';
    strcpy(str, tempStr);
    free(tempStr);
    free(occupied);
}
 
void corrupt(char* str)                                                 // a function which "corrupts" a string by multiplying its each character with a random "corrupting constant" which causes integer overflows and results in a messed up string with
{                                                                       // miscellaneous characters, usually non alphanumeric and non punctuation characters.
    size_t cConst = rand();
    for (size_t i = 0; str[i] != '\0'; i++)
        str[i] = str[i] * cConst;
}
 
unsigned short paramResolve(char** str, char param)                    // function which "resolves" a parameter, i.e. for the specific string passed to it, depending on the parameter, it calls an appropriate function to modify the string.
{                                                                      // note that the f and h don't do anything as they're meant to be the first ones to be activated because they represent either a file name or a URL.
    if (!(*str))                                                       // function returns 0 on success and 1 on failure, failure either by an undefined parameter or the string not existing.
        return 1;
    switch (param)
    {
        case 'r':
            strrev(*str);
            return 0;
        case 'c':
            for (size_t i = 0; (*str)[i] != '\0'; i++)
                (*str)[i] = tolower((*str)[i]);
            return 0;
        case 'C':
            for (size_t i = 0; (*str)[i] != '\0'; i++)
                (*str)[i] = toupper((*str)[i]);
            return 0;
        case 's':
            qsort(*str,strlen(*str),sizeof(char),cmp);
            return 0;
        case 'S':
            scramble(*str);
            return 0;
        case 'd':
            corrupt(*str);
            return 0;
        case 'f':
            return 0;
        case 'h':
            return 0;
        default:
            return 1;
    } //WIP
}   
 
PCTRL strsFormat(char** strsRaw, char* params, size_t* rawCnt)                      // function which formats the strings and is also used for setting flags in PCTRL
{
    PCTRL paramControl = {0,'+',0,0,'+','+',strsRaw};                               // the structure is initialized by setting all the flags to 0 by default, and the "offending parameters" to a placeholder character, a '+' in this case because i don't intend
    if (!params)                                                                    // to use it as a parameter, this is to avoid confusion.
        return paramControl;    
    if (isInS('f',params) && !isInS('h',params))                                    // check for the -f parameter for reading from a file.
    {
        for (size_t i = 0; i < *rawCnt; i++)
            readFromFile(&(paramControl.strsArr[i]));
    }
    else if (isInS('h',params) && !isInS('f',params))                               // check for the -h parameter, for reading from a hyperlink (not yet implemented)
    {
        //for (size_t i = 0; i < *rawCnt; i++)
        //  readFromURL(&(paramControl.strsArr[i]));
        ;
    }
    paramControl.flgIncompParam = (isInS('h',params) && isInS('f',params));         // -f and -h are incompatible parameters, they can't be used together, i might make a separate function for this if i decide to add more mutually incompatible parameters.
    paramControl.incompParam1 = paramControl.flgIncompParam ? 'h' : '+';
    paramControl.incompParam2 = paramControl.flgIncompParam ? 'f' : '+';
    paramControl.flgDupParam = (!noRepeat(params));                                 // check for duplicate parameters via the noRepeat() function and setting the flag.
    if (paramControl.flgIncompParam | paramControl.flgDupParam)
    {
        return paramControl;                                                        // either of those flags being set to 1 abort the program.
    }
    params++;                                                                       // incrementing the pointer pointing to the parameter string to bypass the hyphen.
    for (size_t i = 0; params[i] != '\0'; i++)
    {
        for (size_t j = 0; j < *rawCnt; j++)
        {
            paramControl.flgInvParam = paramResolve(&strsRaw[j],params[i]);         // sets the invalid parameter flag based on the return value of the paramResolve() function
            paramControl.invParam = paramControl.flgInvParam ? params[i] : '+';     // grabbing the offending parameter in case the invalid parameter flag is set.
            if (paramControl.flgInvParam)
            {
                return paramControl;
            }
        }
    }
    return paramControl;
}
 
int main(int argc, char** argv)
{
    srand(time(NULL));                                                              // seeding the random number generator at the start of the program for peak randomness
    unsigned char flgParams;
    char* params;
    unsigned char flgValidation = chkParams(argc, argv, &flgParams, &params);       // first parameter validation.
    if (!flgValidation)
    {
        if (params)
            free(params);
        return 1;
    }
    RAWPKG rawStrPkg = xtrRawStrs(argc,argv,flgParams);                             // extracting non-attribute strings from argv
    if (!rawStrPkg.strsRaw)                                                         // cautious check at every step for an allocation error, just in case.
    {
        if (params)
            free(params);
        fprintf(stderr,"Error: Malloc error.\n");
        return 1;
    }
    PCTRL pControl = strsFormat(rawStrPkg.strsRaw,params,&(rawStrPkg.rawCnt));      // formatting strings and setting the flags, saved into the control structure, though it's a bit redundant that there are 2 structs with a pointer pointing to the same array.
    if (!finParamValidation(pControl, flgParams))                                   // the final parameter validation, in case any of the invalid flags are set, the program aborts and deallocates all the memory.
    {
        relStrArrMem(&pControl.strsArr,rawStrPkg.rawCnt);
        rawStrPkg.strsRaw = NULL;
        if (params)
            free(params);
        params = NULL;
        return 1;
    }
    printStrs(rawStrPkg,params);                                                    // printing the strings / directing them to stdout
    relStrArrMem(&rawStrPkg.strsRaw,rawStrPkg.rawCnt);                              // normally releasing the string array and parameters string at the end of the program.
    if (params)
        free(params);
    return 0;
}

// The program contains no known memory leaks, as of 06/04/2025 I have tested every possible combination of inputs and checked for memory leaks with valgrind and it hasn't detected any.
// This is likely not the final version of the program, there are still things to be added and it would be a shame to waste all this effort and end with only several functional attributes. I first need to learn how to use libcurl before
// adding the -h parameter. Using the Winsock from Windows API or sys/socket.h from POSIX is also a possibility but it looks a lot more painful to learn.
