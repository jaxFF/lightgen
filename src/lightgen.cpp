#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <direct.h> 

// We need to use a local header cause MSVC doesn't include this...
#ifdef PLATFORM_WINDOWS
#include "dirent.h"
#else // Probably a POSIX based machine
#include <dirent.h>
#endif

#define global static

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;
typedef s32 b32;

typedef uintptr_t umm;
typedef intptr_t smm;

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

global b32 IsSpacing(char C) {
    b32 Result = ((C == ' ') || (C == '\t') || (C == '\v') ||(C == '\f'));
    return(Result);
}

global b32 IsEndOfLine(char Value) {
    b32 Result = ((Value == '\n') || (Value == '\r'));
    return Result;
}

global b32 IsWhitespace(char Value, bool IncludeEOL = false) {
    b32 Result = ((Value == ' ') || (Value == '\t') || (Value == '\v') || (Value == '\f') || (IncludeEOL ? IsEndOfLine(Value) : 1));
    return Result;
}

global b32 IsAlphabetical(char Value) {
    b32 Result = (((Value >= 'a') && (Value <= 'z')) || ((Value >= 'A') && (Value <= 'Z')));
    return Result;
}

global b32 IsNumeric(char Value) {
    b32 Result = ((Value >= '0') && (Value <= '9'));
    return Result;
}

global int StringLength(char* String) {
    int Count = 0;
    while (*String++) {
        ++Count;
    }
    return Count;
}

global bool StringsMatch(char* A, char* B) {
    while (*A && *B) {
        if (*A != *B){
            return false;
        }

        ++A;
        ++B;
    }

    if (*A != *B){
        return false;
    } else {
        return true;
    }
}

// A structure that encapsulates a non-terminated buffer
struct string {
    u8* Data;
    umm Count;
};

global string make_string(char* String) {
    string Result = {};
    Result.Data = (u8*)String;
    Result.Count = StringLength(String);

    return Result;
}

global u8* Advance(string* String, umm Count) {
    u8 *Result = 0;
    
    if(String->Count >= Count) {
        Result = String->Data;
        String->Data += Count;
        String->Count -= Count;
    } else {
        String->Data += String->Count;
        String->Count = 0;
    }
    
    return(Result);    
}

global void ZeroSize(void* Ptr, size_t Size) {
    u8* Byte = (u8*)Ptr;
    // todo(jax): @Speed: Check performance of the two code blocks
#if 0
    for (size_t i = 0; i < Size; ++i) {
        *Byte++ = 0;
    }
#else
    while (Size--) {
        *Byte++ = 0;
    }
#endif
}

global void* MemoryCopy(void* _Dest, void* _Source, size_t Size) {
    if (_Source == 0) {
        ZeroSize(_Dest, Size);
        return _Dest;
    }

    u8* Source = (u8*)_Source;
    u8* Dest = (u8*)_Dest;

    while (Size--) {
        *Dest++ = *Source++;
    }

    return _Dest;
}

#ifdef PLATFORM_WINDOWS
#define PATH_SEPERATOR '\\'
#define OPPOSITE_PATH_SEPERATOR '/'
#elif (PLATFORM_MACOS | PLATFORM_LINUX)
#define PATH_SEPERATOR '/'
#define OPPOSITE_PATH_SEPERATOR '\\'
#endif

global void ConvertPathSlashes(char* Path, char Delim = PATH_SEPERATOR, char OppDelim = OPPOSITE_PATH_SEPERATOR) {
    for (char* At = Path; *At != 0; ++At) {
		if (*At == OppDelim)
			*At = Delim;
	}	
}

global char* StringCopy(char* String) {
    char* Result = (char*)malloc(sizeof(char) * (StringLength(String) + 1));
    MemoryCopy(Result, String, sizeof(char) * (StringLength(String) + 1));

    return Result;
}

global char* StringAppend(char* Dest, size_t DestSize, char* Source) {
    // todo(jax): Do we need to terminate annd or allocate needed memory?
    char* _Dest = StringCopy(Dest);
    Dest = (char*)calloc(DestSize, (1 + StringLength(Source) * sizeof(char)));
    MemoryCopy(Dest, _Dest, DestSize + 1);
    MemoryCopy(Dest + StringLength(Dest), Source, DestSize + 1);
    return Dest;
}

global char* Substring(char* Source, char* String) {
    while (*Source) {
        char* Begin = Source;
        char* Pattern = String;

        // If first character of sub string match, check for whole string
        while (*Source && *Pattern && *Source == *Pattern) {
            Source++;
            Pattern++;
        }

        // If complete sub string match, return starting address 
        if (!*Pattern)
            return Begin;

        Source = Begin + 1;
    }
    return NULL;
}

#include "tokenizer.h"
#include "tokenizer.cpp"

int main(int ArgCount, char** Args) {
    if (ArgCount >= 2) {
        DIR *dir;
        struct dirent *ent;

        char* Directory = (char*)malloc(sizeof(char) * 256);
        _getcwd(Directory, 256);

        Directory = StringAppend(Directory, 256, "\\");
        Directory = StringAppend(Directory, 256, Args[1]);

        ConvertPathSlashes(Directory);
        //char* fullPath = realpath(Args[2], cwd);
        if ((dir = opendir (Directory)) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                printf ("%s\n", ent->d_name);
            }
            closedir (dir);
        } else {
            perror ("");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Usage: %s <site directory>\n", Args[0]);
    }

    return 1;
}