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

// Forward declarations for macro code
void* MemoryCopy(void* _Dest, void* _Source, size_t Size);
char* FindFirstChar(char* String, int _Char);

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

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Assert(Expression) if (!(Expression)) { *(int*)(0); }

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

#define AllocStruct(type) (type*)malloc(sizeof(type))
#define AllocCopy(size, source) MemoryCopy(malloc(size), source, size)

#define __BASENAME__ (FindFirstChar(__FILE__, '/') ? FindFirstChar(__FILE__, '/') + 1 : FindFirstChar(__FILE__, '\\') ? FindFirstChar(__FILE__, '\\') + 1 : __FILE__)

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

global b32 IsSymbolic(char Value) {
    b32 Result = (Value == '*' || Value == '|' 
                || Value == ',' || Value == '.'
                || Value == ';' || Value == ':'
                || Value == '=' || Value == '!' || Value == '?');
    return Result;
}

global b32 IsPunctuation(char Value) {
    b32 Result = (Value == ',' || Value == '.' || Value == '!' || Value == '?');
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

// note: _Char is expected to be an ASCII character
global char* FindFirstChar(char* String, int _Char) {
    char Char = (char)_Char;
    while (*String != Char) {
        if (!*String++) {
            return NULL;
        }
    }
    return (char*)String;
}

// strstr()
global char* FindFirstString(char* _Source, char* _String) {
    while (*_Source != 0) {
        char* Source, *String;
        for (Source = _Source, String = _String; *String != 0 && (*Source == *String); ++Source, ++String);

        if (*String == 0) {
            return _Source;
        }

        _Source++;
    }

    return nullptr;
}

global void SolveRelativeDirectory(char* Path) {
    ConvertPathSlashes(Path, '/', PATH_SEPERATOR);
    for (;;) {
        char* Relative = FindFirstString(Path + 1, "/../");

        char* Previous = Relative - 1;
        while ((Previous > Path) && (Previous[0] != '/')) {
            Previous--;
        }

        char* In = Relative + 3;
        for (;;) {
            *Previous = *In;
            if (In[0]) {
                ++Previous;
                ++In;
            }
            else break;
        }

        if (!Substring(Path, "../"))
            break;
    }
}

#include "tokenizer.h"
#include "tokenizer.cpp"
#include "shared.cpp"

//
// Stream code
//

struct stream_chunk {
	char* File;
	u32 LineNumber;

	void* Contents;
	size_t ContentsSize;

	stream_chunk* Next;
};

struct stream {
	stream* Errors;

	void* Contents;
	size_t ContentsSize;

	uint32_t BitCount;
	uint32_t BitBuf;

	bool HasUnderflowed;

	stream_chunk* First;
	stream_chunk* Last;
};

#define Outf(...) Outf_((char*)__BASENAME__, __LINE__, __VA_ARGS__)
void Outf_(char* FileName, uint32_t LineNumber, stream* Dest, char* Format, ...);

stream_chunk* AppendChunk(stream* Stream, size_t Size, void* Contents) {
	stream_chunk* Chunk = AllocStruct(stream_chunk);

	Chunk->ContentsSize = Size;
	Chunk->Contents = Contents;
	Chunk->Next = 0;

	Stream->Last =
		(((Stream->Last) ? Stream->Last->Next : Stream->First) = Chunk);

	return Chunk;
}

void RefillIfNeccessary(stream* file) {
	// todo(jax): use free list to recycle chunks if we ever care

	if ((file->ContentsSize == 0) && file->First) {
		stream_chunk* This = file->First;
		file->ContentsSize = This->ContentsSize;
		file->Contents = This->Contents;
		file->First = This->Next;
	}
}

#define Consume(Contents, Type) (Type*)ConsumeSize(Contents, sizeof(Type))
void* ConsumeSize(stream* file, u32 Size) {
	void* Result = 0;

	RefillIfNeccessary(file);

	if (file->ContentsSize >= Size) {
		Result = file->Contents;
		file->Contents = (u8*)file->Contents + Size;
		file->ContentsSize -= Size;
	}
	else {
		Outf(file->Errors, "FILE ERROR: underflown buffer!\n");
		file->ContentsSize = 0;
		file->HasUnderflowed = true;
	}

	Assert(!file->HasUnderflowed);

	return Result;
}


u32 PeekBits(stream* Buf, u32 BitCount) {
	Assert(BitCount <= 32);

	u32 Result = 0;
	while ((Buf->BitCount < BitCount) && !Buf->HasUnderflowed) {
		u32 Byte = *Consume(Buf, u8);
		Buf->BitBuf |= (Byte << Buf->BitCount);
		Buf->BitCount += 8;
	}

	Result = Buf->BitBuf & ((1 << BitCount) - 1);

	return Result;
}

void DiscardBits(stream* Buf, u32 BitCount) {
	Buf->BitCount -= BitCount;
	Buf->BitBuf >>= BitCount;
}

u32 ConsumeBits(stream* Buf, u32 BitCount) {
	u32 Result = PeekBits(Buf, BitCount);
	DiscardBits(Buf, BitCount);

	return Result;
}

void FlushByte(stream* Buf) {
	u32 FlushCount = (Buf->BitCount % 8);
	ConsumeBits(Buf, FlushCount);
}

u32 ReverseBits(u32 V, u32 BitCount) {
	u32 Result = 0;

	// Speed(jax): Really bad way to flip these bits
	for (u32 BitIndex = 0; BitIndex <= (BitCount / 2); ++BitIndex) {
		u32 InversionOp = (BitCount - (BitIndex + 1));
		Result |= ((V >> BitIndex) & 0x1) << InversionOp;
		Result |= ((V >> InversionOp) & 0x1) << BitIndex;
	}

	return Result;
}

global void Outf_(char* FileName, uint32_t LineNumber, stream* Dest, char* Format, ...) {
	va_list ArgList;

	char Buffer[1024];

	va_start(ArgList, Format);
	size_t Size = FormatArgList(sizeof(Buffer), Buffer, Format, ArgList);
	va_end(ArgList);

	void* Contents = AllocCopy(Size, (void*)Buffer);
	stream_chunk* Chunk = AppendChunk(Dest, Size, Contents);
	Chunk->File = FileName;
	Chunk->LineNumber = LineNumber;
}

global stream OnDemandMemoryStream(stream* Errors = 0) {
	stream Result = {};
	Result.Errors = Errors;

	return Result;
}

global void DumpStreamToCRT(stream* Source, FILE* Dest = stdout) {
	char Append[256];
	for (stream_chunk* Chunk = Source->First; Chunk; Chunk = Chunk->Next) {
		sprintf(Append, "%s(%d): ", Chunk->File, Chunk->LineNumber);
		fwrite(Append, StringLength(Append), 1, Dest);
		fwrite(Chunk->Contents, Chunk->ContentsSize, 1, Dest);
	}
}

//
//
//

global char* ReadEntireFileIntoMemory(char* FileName) {
    char* Result = 0;

    FILE* File = fopen(FileName, "r");
	if (File) {
		fseek(File, 0, SEEK_END);
		size_t FileSize = ftell(File);
		fseek(File, 0, SEEK_SET);

		Result = (char*)malloc(FileSize + 1);
		fread(Result, FileSize, 1, File);
		Result[FileSize] = 0;

		fclose(File);
	}

    return Result;
}

// Data associated with a page file
struct PageParseData {

};

PageParseData ParsePageFile(tokenizer Tokenizer, PageParseData* Data) {
    PageParseData Result = {};

    b32 Parsing = true;
    while (Parsing) {
        token Token = GetToken(&Tokenizer);
        switch (Token.Type) {
            case Token_EndOfStream: {
                Parsing = false;
            } break;

            case Token_Pound: {
                printf("");
            } break;

            case Token_Identifier: {
                printf("%s\n", Token.String);
            } break;

            case Token_String: {
                printf("%s\n", Token.String);
            } break;

            case Token_Unknown:
            default: {
                printf("");
            } break;
         }
    }

    return Result;
}
// Data associated with a website manifest file
struct ManifestParseData {
    char* SiteHeader;
    char* SiteFooter;
};

ManifestParseData ParseManifestFile(tokenizer Tokenizer, ManifestParseData* Data) {
    ManifestParseData Result = {};

    b32 Parsing = true;
    while (Parsing) {
        token Token = GetToken(&Tokenizer);
        switch (Token.Type) {
        case Token_EndOfStream: {
            Parsing = false;
        } break;

        case Token_Pound: {
            printf("");
        } break;

        case Token_Identifier: {
            printf("%s\n", Token.String);
        } break;

        case Token_String: {
            printf("%s\n", Token.String);
        } break;

        case Token_Unknown:
        default: {
            printf("");
        } break;
        }
    }

    return Result;
}

int HandleDirectory(char* Path) {
    DIR* Dir;
    struct dirent* Entry;

    ConvertPathSlashes(Path);
    if ((Dir = opendir(Path)) != NULL) {
        while ((Entry = readdir(Dir)) != NULL) {
            // Get the absolute filename, pretty hacky code...
            char* Filename = StringCopy(Path);
            Filename = StringAppend(Filename, 256, "\\");
            Filename = StringAppend(Filename, 256, Entry->d_name);

            if (Entry->d_type == DT_DIR) {   
                if (Substring(Entry->d_name, ".")) {
                    continue;
                }

                HandleDirectory(Filename);
            } if (Substring(Entry->d_name, ".manifest")) {
                tokenizer Tokenizer = Tokenize(make_string(ReadEntireFileIntoMemory(Filename)), Filename);
                ParseManifestFile(Tokenizer, 0);
            }
            else if (Substring(Entry->d_name, ".lgs")) {
                tokenizer Tokenizer = Tokenize(make_string(ReadEntireFileIntoMemory(Filename)), Filename);
                ParsePageFile(Tokenizer, 0);
            }

            printf("%s\n", Filename);
        }
        closedir(Dir);
    }
    else {
        perror("");
        return EXIT_FAILURE;
    }

    return 0;
}

int main(int ArgCount, char** Args) {
    if (ArgCount >= 2) {
        // This code is pretty bad w/e
        char* Directory = (char*)malloc(sizeof(char) * 256);
        _getcwd(Directory, 256);

        Directory = StringAppend(Directory, 256, "\\");
        Directory = StringAppend(Directory, 256, Args[1]);

        SolveRelativeDirectory(Directory);
        ConvertPathSlashes(Directory);
        HandleDirectory(Directory);
    } else {
        fprintf(stderr, "Usage: %s <site directory>\n", Args[0]);
        return 1;
    }

    return 0;
}