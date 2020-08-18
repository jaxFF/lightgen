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
#define ZeroStruct(type) ZeroSize(type, sizeof(type))

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

global char* FindLastChar(char* String, int _Char) {
    char Char = (char)_Char;
    if (Char == 0)
        return FindFirstChar(String, _Char);

    char Found;
    char* Ptr = nullptr;
    while ((Ptr = FindFirstChar(String, _Char)) != 0) {
        Found = *Ptr;
        String = Ptr + 1;
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
    
    ConvertPathSlashes(Path);
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
        if (Chunk->File != 0) {
		    sprintf(Append, "%s(%d): ", Chunk->File, Chunk->LineNumber);
		    fwrite(Append, StringLength(Append), 1, Dest);
        }
		fwrite(Chunk->Contents, Chunk->ContentsSize, 1, Dest);
	}
}

//
//
//

#define ParseError(Stream, Tokenizer, String, ...) {\
    Outf_((char*)Tokenizer.Filename, Tokenizer.LinesCount, Stream, "Parse Error: ");\
    Outf_(0, 0, Stream, String, __VA_ARGS__); \
    Outf_(0, 0, Stream, "\n"); \
}

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

//
// Page parsing
//

enum ASTPageToken {
    ASTPageToken_String,
    ASTPageToken_NewLine,

    ASTPageToken_Header,
    ASTPageToken_BlockQuote,

    ASTPageToken_LeftBold,
    ASTPageToken_LeftUnderline,
    ASTPageToken_LeftStrikethrough,
    ASTPageToken_LeftItalics,

    ASTPageToken_RightBold,
    ASTPageToken_RightUnderline,
    ASTPageToken_RightStrikethrough,
    ASTPageToken_RightItalics
};

struct AST_PageNode {
    AST_PageNode* Next;
    ASTPageToken Type;
    string Text;
    union {
        struct {
            s32 HCount;
        } header;

        struct {
            AST_PageNode* First;
        } unordered_list;

        struct {
            AST_PageNode* First;
        } ordered_list;
    };
};

// Data associated with a page file
struct PageParseArguments {
    stream* Out;

    char* Filename;
    char* BaseFilename;
    char* HTMLOutputFilename;
    char* FilenameNoExtension;
};

struct ProcessedPage {
    AST_PageNode* Root;

    char* Filename;
    char* BaseFilename;
    char* HTMLOutputFilename;
    char* FilenameNoExtension;
};

// Data associated with a website manifest file
struct ManifestParseArguments {
    stream* Out;

    char* Filename;
};

struct ProcessedManifest {
    char* Filename;

    char* SiteTitle;
    char* SiteAbstract;
    char* SiteURL;

    char* SiteHeader;
    char* SiteFooter;
};

global AST_PageNode* AllocateASTPageNode() {
    AST_PageNode* Node = AllocStruct(AST_PageNode);
    ZeroStruct(Node);
    return Node;
}

ProcessedPage ParsePageFile(tokenizer Tokenizer, PageParseArguments* Data) {
    AST_PageNode* Root = 0;
    AST_PageNode** Last = &Root;

    AST_PageNode* HeaderNode = 0;

    ProcessedPage Result = {};
    Result.Filename = StringCopy(Data->Filename);
    Result.BaseFilename = StringCopy(Data->BaseFilename);
    Result.FilenameNoExtension = StringCopy(Data->FilenameNoExtension);
    Result.HTMLOutputFilename = StringCopy(Data->HTMLOutputFilename);

    s32 HeaderCount = 0; // todo: Create parse errors for this. (if it's above 6 I think)

    b32 BoldLeft = false;
    b32 UnderlineLeft = false;
    b32 StrikethroughLeft = false;
    b32 ItalicsLeft = false;

    b32 Parsing = true;
    while (Parsing) {
        token Token = GetToken(&Tokenizer);

        switch (Token.Type) {
            case Token_EndOfStream: {
                Parsing = false;
            } break;

            case Token_EndOfLine: {
                AST_PageNode* Node = AllocateASTPageNode();
                Node->Type = ASTPageToken_NewLine;
                *Last = Node;
                Last = &(*Last)->Next;
            } break;

            case Token_Asterisk: {
                if (BoldLeft && TokenEquals(GetToken(&Tokenizer), Token_Asterisk)) { // This is a bold right
                    // Actually increment the tokenizer now that we've checked
                    //Token = GetToken(&Tokenizer);
                    
                    BoldLeft = false;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_RightBold;
                    *Last = Node;
                    Last = &(*Last)->Next;
                } else if (TokenEquals(GetToken(&Tokenizer), Token_Asterisk)) { // This is a bold left
                    // Actually increment the tokenizer now that we've checked
                    //Token = GetToken(&Tokenizer);
                    
                    BoldLeft = true;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_LeftBold;
                    *Last = Node;
                    Last = &(*Last)->Next;
                } else if (ItalicsLeft) { // This is a italics right because one Asterisk
                    ItalicsLeft = false;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_RightItalics;
                    *Last = Node;
                    Last = &(*Last)->Next;
                } else { // This is a italics left because one Asterisk
                    ItalicsLeft = true;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_LeftItalics;
                    *Last = Node;
                    Last = &(*Last)->Next;
                }
            } break;

            case Token_Underline: {
                if (UnderlineLeft && TokenEquals(GetToken(&Tokenizer), Token_Underline)) {
                    // Actually increment the tokenizer now that we've checked
                    //Token = GetToken(&Tokenizer);

                    // This is a underline right
                    UnderlineLeft = false;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_RightUnderline;
                    *Last = Node;
                    Last = &(*Last)->Next;
                } else if (TokenEquals(GetToken(&Tokenizer), Token_Underline)) {
                    // Actually increment the tokenizer now that we've checked
                    //Token = GetToken(&Tokenizer);
                    
                    UnderlineLeft = true;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_LeftUnderline;
                    *Last = Node;
                    Last = &(*Last)->Next;
                } else if (ItalicsLeft) {  // This is an italics right
                    ItalicsLeft = false;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_RightItalics;
                    *Last = Node;
                    Last = &(*Last)->Next;
                } else { // This is an italics left
                    ItalicsLeft = true;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_LeftItalics;
                    *Last = Node;
                    Last = &(*Last)->Next;
                }
            } break;

            case Token_Tilde: {
                if (StrikethroughLeft && TokenEquals(GetToken(&Tokenizer), Token_Tilde)) { // This is a strikethrough right
                    StrikethroughLeft = false;

                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_RightStrikethrough;
                    *Last = Node;
                    Last = &(*Last)->Next;
                } else if (TokenEquals(GetToken(&Tokenizer), Token_Tilde)) { // This is a strikethrough left
                    StrikethroughLeft = true;
                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_LeftStrikethrough;
                    *Last = Node;
                    Last = &(*Last)->Next;
                }
            } break; 

            case Token_Pound: {
                if (HeaderCount == 0) { // Define a new header since we're at the start
                    AST_PageNode* Node = AllocateASTPageNode();
                    Node->Type = ASTPageToken_Header;
                    Node->header.HCount = HeaderCount;
                    *Last = Node;
                    Last = &(*Last)->Next;

                    HeaderNode = Node;
                }

                HeaderCount++;
            } break;

            case Token_Identifier: {
                printf("Ident%s\n", Token.String);

                if (HeaderCount != 0) { // Update the nodes count and reset since the header is defined now
                    HeaderNode->header.HCount = HeaderCount;
                    HeaderCount = 0;
                }
                
                AST_PageNode* Node = AllocateASTPageNode();
                Node->Type = ASTPageToken_String;
                Node->Text = make_string(Token.String);
                *Last = Node;
                Last = &(*Last)->Next;
            } break;

            case Token_String: {
                printf("%s\n", Token.String);
            } break;

            case Token_Unknown:
            default: {
                if (StringsMatch((CharToString(Token)).String, ">")) {
                    Token = PeekTokenSkipSpace(&Tokenizer);
                }
                printf("");
            } break;
         }
    }

    Result.Root = Root;
    return Result;
}

global void OutputHTMLHeader(FILE* Out, ProcessedPage* Page) {
    fprintf(Out, "<!DOCTYPE html>\n");
    fprintf(Out, "<html lang=\"en\">\n");
    fprintf(Out, "<meta http-equiv=\"content-type\"; content=\"text/html\"; charset=\"UTF-8\">\n");
    fprintf(Out, "<head>\n");
    fprintf(Out, "<title>Lightgen</title>\n");
    fprintf(Out, "</head>\n");
    fprintf(Out, "<body>\n");
}

global void OutputHTMLFooter(FILE* Out, ProcessedPage* Page) {
    fprintf(Out, "</body>\n");
    fprintf(Out, "</html>\n");
}

global void OutputHTMLFromPageSyntaxTree(AST_PageNode* Node, FILE* Out, ProcessedPage* Pages, s32 PageCount) {
    b32 Paragraph = false;

    AST_PageNode* Prev = 0;
    for (; Node; Prev = Node, Node = Node->Next) {
        switch (Node->Type) {
            case ASTPageToken_Header: {
                fprintf(Out, "<h%d>%.*s</h%d>\n", Node->header.HCount, (int)Node->Text.Count, Node->Text.Data, (int)Node->header.HCount);
            } break;
        }
    }
}

// Manifest format will look like https://github.com/jgaa/stbl/blob/master/examples/bootstrap/stbl.conf
ProcessedManifest ParseManifestFile(tokenizer Tokenizer, ManifestParseArguments* Data) {
    ProcessedManifest Result = {};
    Result.Filename = Data->Filename;

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

global u32 PageCount;
global ProcessedPage Pages[8096];
global ProcessedManifest SiteManifest;

ProcessedPage ProcessPage(char* Filename, tokenizer Tokenizer, PageParseArguments* Args) {
    Args->Out = &OnDemandMemoryStream();
    ProcessedPage Result = ParsePageFile(Tokenizer, Args);
    DumpStreamToCRT(Args->Out);
    return Result;
}

ProcessedManifest ProcessManifest(char* Filename, tokenizer Tokenizer, ManifestParseArguments* Args) {
    Args->Out = &OnDemandMemoryStream();
    ProcessedManifest Result = ParseManifestFile(Tokenizer, Args);
    DumpStreamToCRT(Args->Out);
    return Result;
}

int ProcessDirectory(char* Path) {
    DIR* Dir;
    struct dirent* Entry;

    ConvertPathSlashes(Path);
    if ((Dir = opendir(Path)) != NULL) {
        while ((Entry = readdir(Dir)) != NULL) {
            // Get the absolute filename, pretty hacky code...
            char* Filename = StringCopy(Path);
            Filename = StringAppend(Filename, 256, "\\");
            Filename = StringAppend(Filename, 256, Entry->d_name);
            ConvertPathSlashes(Path);

            if (Entry->d_type == DT_DIR) {   
                if (Substring(Entry->d_name, ".")) {
                    continue;
                }

                ProcessDirectory(Filename);
            } else if (Substring(Entry->d_name, ".manifest")) {
                string FileData = make_string(ReadEntireFileIntoMemory(Filename));
                tokenizer Tokenizer = Tokenize(FileData, Filename);

                ManifestParseArguments Args = {};
                Args.Filename = Filename;
                ProcessedManifest Manifest = ProcessManifest(Filename, Tokenizer, &Args);

                SiteManifest = Manifest;
            } else if (Substring(Entry->d_name, ".lgs")) {
                string FileData = make_string(ReadEntireFileIntoMemory(Filename));
                tokenizer Tokenizer = Tokenize(FileData, Filename);

                char* BaseFilename = Entry->d_name;
                char* FilenameNoExtension = StringCopy(BaseFilename);
                char HTMLOutputFilename[256] = {};

                // Remove the Dot from FilenameNoExtension
                char* Dot = FindLastChar(FilenameNoExtension, '.');
                Dot[-1] = 0; 

                sprintf(HTMLOutputFilename, "%s/../", Path);
                SolveRelativeDirectory(HTMLOutputFilename);
                sprintf(HTMLOutputFilename, "%sgenerated\\%s.html", HTMLOutputFilename, FilenameNoExtension);
                ConvertPathSlashes(HTMLOutputFilename);

                PageParseArguments Args = {};
                Args.Filename = Filename;
                Args.BaseFilename = BaseFilename;
                Args.FilenameNoExtension = FilenameNoExtension;
                Args.HTMLOutputFilename = HTMLOutputFilename;
                ProcessedPage Page = ProcessPage(Filename, Tokenizer, &Args);

                if (PageCount < ArrayCount(Pages)) {
                    Pages[PageCount++] = Page;
                }
                printf("%s\n", Filename);
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

// todo: Tokenization and page parsing needs a massive cleanup. Everything there is temporary.

int main(int ArgCount, char** Args) {
    if (ArgCount >= 2) {
        // This code is pretty bad w/e
        char* Directory = (char*)malloc(sizeof(char) * 256);
        _getcwd(Directory, 256);

        Directory = StringAppend(Directory, 256, "\\");
        Directory = StringAppend(Directory, 256, Args[1]);

        SolveRelativeDirectory(Directory);
        ConvertPathSlashes(Directory);
        ProcessDirectory(Directory);

        // Generate the html files
        for (int i = 0; i < PageCount; ++i) {
            ProcessedPage* File = (Pages + i);
            FILE* Out = fopen(File->HTMLOutputFilename, "w+");
            if (!Out) {
                printf("Failed to open '%s'.", File->HTMLOutputFilename);
            }

            if (File->HTMLOutputFilename) {
                OutputHTMLHeader(Out, File);
                OutputHTMLFromPageSyntaxTree(File->Root, Out, Pages, PageCount);
                OutputHTMLFooter(Out, File);
            }

            fclose(Out);
        }
    } else {
        fprintf(stderr, "Usage: %s <site directory>\n", Args[0]);
        return 1;
    }

    return 0;
}