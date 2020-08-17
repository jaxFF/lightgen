/* ============ Copyright (c) 2020 by Asteria Development, All rights reserved. ============
 *  Creator: Jaxon Laing
 *
 * Unauthorized copying or distrubution of this file, via any medium is prohibited
 * without the express permission of the project author or the terms of the license.
 *
 * This file is subject to the full license information defined in file
 * 'LICENSE', which is located in the root of this source code package.
 */

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
    Token_Period, // .
    Token_Equals, // =
    Token_Comma, // ,
    Token_Or, // |
    Token_Pound, // #
    Token_Underline, // _
    Token_Backtick, // `
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
    size_t TextLength;
    string Text;
    union {
        char* String;
        f32 Float;
        s32 Integer;
    };
};

struct tokenizer {
    char* Filename;
    u32 LinesCount;
    u32 TokensCount;
    u32 TotalCount;

    string Input;
    char* At;
};

b32 IsTokenValid(token Token);
b32 TokenEquals(token Token, char* Match);
b32 TokenEquals(token Token, token_type Type);
token GetToken(tokenizer* Tokenizer);
token PeekToken(tokenizer* Tokenizer, int PeekCount);
token PeekAheadToken(tokenizer* Tokenizer);
token PeekTokenSkipSpace(tokenizer* Tokenizer);
tokenizer Tokenize(string Data, char* Filename);

#endif