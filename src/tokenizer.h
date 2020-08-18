#ifndef TOKENIZER_H
#define TOKENIZER_H

enum token_type {
    Token_Unknown,

    Token_OpenParen, // (
    Token_CloseParen, // )
    Token_Colon, // :
    Token_Semicolen, // ;
    Token_Asterisk, // *
    Token_OpenBracket, // [
    Token_CloseBracket, // ]
    Token_OpenBrace, // {
    Token_CloseBrace, // }
    Token_Equals,
    Token_Comma,
    Token_Or,
    Token_Pound,
    Token_Underscore, // _
    Token_Tilde, // ~

    Token_String,
    Token_Identifier,
    Token_Literal, // Number

    Token_Space,
    Token_EndOfLine,
    Token_Comment,

    Token_EndOfStream,
};

struct token {
    char* Filename;

    token_type Type;
    string Text;
    f32 Float;
    s32 Integer;
};

struct tokenizer {
    char* Filename;
    u32 LineCount;
    u32 TokenCount;
    u32 ColumnCount;

    string Input;
    char At[2];
};

b32 IsTokenValid(token Token);
b32 TokenEquals(token Token, char* Match);
b32 TokenEquals(token Token, token_type Type);
token GetTokenRaw(tokenizer* Tokenizer);
token GetToken(tokenizer* Tokenizer);
token PeekToken(tokenizer* Tokenizer, int PeekCount);
token PeekTokenRaw(tokenizer* Tokenizer);
token PeekToken(tokenizer* Tokenizer);
token PeekTokenSkipSpace(tokenizer* Tokenizer);
tokenizer Tokenize(string Data, char* Filename);

#endif