/* ============ Copyright (c) 2020 by Asteria Development, All rights reserved. ============
 *  Creator: Jaxon Laing
 *
 * Unauthorized copying or distrubution of this file, via any medium is prohibited
 * without the express permission of the project author or the terms of the license.
 *
 * This file is subject to the full license information defined in file
 * 'LICENSE', which is located in the root of this source code package.
 */

b32 IsTokenValid(token Token) {
    b32 Result = (Token.Type != Token_Unknown);
    return Result;
}

b32 TokenEquals(token Token, char* Match) {
    b32 Result = StringsMatch(Token.String, Match);
    return Result;
}

b32 TokenEquals(token Token, token_type Type) {
    b32 Result = (Token.Type == Type);
    return Result;
}

void Refill(tokenizer *Tokenizer){
    if(Tokenizer->Input.Count == 0) {
        Tokenizer->At[0] = 0;
        Tokenizer->At[1] = 0;
    } else if(Tokenizer->Input.Count == 1) {
        Tokenizer->At[0] = Tokenizer->Input.Data[0];
        Tokenizer->At[1] = 0;
    } else {
        char C0 = Tokenizer->Input.Data[0];
        char C1 = Tokenizer->Input.Data[1];
        Tokenizer->At[0] = C0;
        Tokenizer->At[1] = C1;
    }
}

void AdvanceInput(tokenizer* Tokenizer, u32 Count) {
    Tokenizer->TotalCount += Count;
    Advance(&Tokenizer->Input, Count);
    Refill(Tokenizer);
}

tokenizer Tokenize(string Data, char* Filename) {
    tokenizer Result = {};
    
    Result.Filename = Filename;
    Result.LinesCount = 1;
    Result.TokensCount = 1;   
    Result.TotalCount = 1;    
    Result.Input = Data;
    Result.At = (char*)malloc(2);
    Refill(&Result);
    
    return(Result);
}

tokenizer Tokenize(char* Data);
tokenizer Tokenize(char* Filename);

token CharToString(token Token) {
    Token.String = (char*)malloc(sizeof(char) * 1);
    sprintf(Token.String, "%.*s", 1, Token.Text.Data);
    return Token;
}

token GetToken(tokenizer* Tokenizer) {
	token Token = {};
    Token.Filename = Tokenizer->Filename;
	Token.TextLength = 1;
	Token.Text = Tokenizer->Input;

	char C = Tokenizer->At[0];
    AdvanceInput(Tokenizer, 1);
	switch (C) {
        case '\0': { Token.Type = Token_EndOfStream; } break;
        case '\n': { Tokenizer->LinesCount++; } break;

        case '(': { Token.Type = Token_OpenParen; CharToString(Token); } break;
        case ')': { Token.Type = Token_CloseParen; CharToString(Token); } break;
        case ':': { Token.Type = Token_Colon; CharToString(Token); } break;
        case ';': { Token.Type = Token_Semicolen; CharToString(Token); } break;
        case '*': { Token.Type = Token_Asterisk; CharToString(Token); } break;
        case '[': { Token.Type = Token_OpenBracket; CharToString(Token); } break;
        case ']': { Token.Type = Token_CloseBracket; CharToString(Token); } break;
        case '{': { Token.Type = Token_OpenBrace; CharToString(Token); } break;
        case '}': { Token.Type = Token_CloseBrace; CharToString(Token); } break;
        case '=': { Token.Type = Token_Equals; CharToString(Token); } break;
        case '.': { Token.Type = Token_Period; CharToString(Token); } break;
        case ',': { Token.Type = Token_Comma; CharToString(Token); } break;
        case '|': { Token.Type = Token_Or; CharToString(Token); } break;
        case '#': { Token.Type = Token_Pound; CharToString(Token); } break;
        case '_': { Token.Type = Token_Underline; CharToString(Token); } break;
        case '~': { Token.Type = Token_Tilde; CharToString(Token); } break;
        
        case '`': { Token.Type = Token_Backtick; CharToString(Token); } break;

        case '"': { // note(jax): We've got a string
			Token.Type = Token_String;
            while(Tokenizer->At[0] && Tokenizer->At[0] != '"') {
                if ((Tokenizer->At[0] == '\\') && (Tokenizer->At[1])) {
                    AdvanceInput(Tokenizer, 1);
                }

                AdvanceInput(Tokenizer, 1);;
            }

            if (Tokenizer->At[0] == '"') {
                AdvanceInput(Tokenizer, 1);
            }

            // Speed: This block of code is pretty hacky...
            if (Token.Text.Count &&
                (Token.Text.Data[0] == '"')) {
                ++Token.Text.Data;
                Token.Text.Count -= 2;


                __int64 IndentLength = (Tokenizer->Input.Data - Token.Text.Data) - 1;
                Token.String = (char*)malloc(IndentLength);
                sprintf(Token.String, "%.*s", (int)IndentLength, Token.Text.Data);
            }
        } break;

        default: {
            if (IsSpacing(C)) {
                Token.Type = Token_Space;
                while (IsSpacing(Tokenizer->At[0])) {
                    AdvanceInput(Tokenizer, 1);
                }
            } else if (IsEndOfLine(C)) {
                Token.Type =  Token_EndOfLine;
                if(((C == '\r') &&
                    (Tokenizer->At[0] == '\n')) ||
                   ((C == '\n') &&
                    (Tokenizer->At[0] == '\r'))) {
                    AdvanceInput(Tokenizer, 1);
                }
                
                ++Tokenizer->LinesCount;
            } else if((C == '/') && (Tokenizer->At[0] == '/')) {
                Token.Type = Token_Comment;
                
                AdvanceInput(Tokenizer, 2);
                while(Tokenizer->At[0] && !IsEndOfLine(Tokenizer->At[0])) {
                    AdvanceInput(Tokenizer, 1);
                }
            } else if((C == '/') &&
                    (Tokenizer->At[0] == '*')) {
                Token.Type = Token_Comment;
                
                AdvanceInput(Tokenizer, 2);
                while(Tokenizer->At[0] && !((Tokenizer->At[0] == '*') 
                    && (Tokenizer->At[1] == '/'))) {
                    if(((Tokenizer->At[0] == '\r') && (Tokenizer->At[1] == '\n')) 
                        || ((Tokenizer->At[0] == '\n') && (Tokenizer->At[1] == '\r'))) {
                        AdvanceInput(Tokenizer, 1);
                    }
                    
                    if (IsEndOfLine(Tokenizer->At[0])) {
                        ++Tokenizer->LinesCount;
                    }
                    
                    AdvanceInput(Tokenizer, 1);
                }
                
                if(Tokenizer->At[0] == '*') {
                    AdvanceInput(Tokenizer, 2);
                }
            } else if (IsAlphabetical(C)) {
				Token.Type = Token_Identifier;
                while (IsAlphabetical(Tokenizer->At[0]) || IsNumeric(Tokenizer->At[0]) || IsPunctuation(Tokenizer->At[0])) {
                    AdvanceInput(Tokenizer, 1);
                }

                __int64 IndentLength = (Tokenizer->Input.Data - Token.Text.Data);
                Token.String = (char*)malloc(IndentLength);
                sprintf(Token.String, "%.*s", (int)IndentLength, Token.Text.Data);
            } else if (IsNumeric(C)) {
                f32 Number = (f32)(C - '0');
                
                while (IsNumeric(Tokenizer->At[0])) {
                    f32 Digit = (f32)(Tokenizer->At[0] - '0');
                    Number = 10.0f*Number + Digit;
                    AdvanceInput(Tokenizer, 1);
                }
                
                // This float detection code is copy pasted, I have no idea how it works!
                if (Tokenizer->At[0] == '.') {
                    AdvanceInput(Tokenizer, 1);
                    f32 Coefficient = 0.1f;
                    while (IsNumeric(Tokenizer->At[0])) {
                        f32 Digit = (f32)(Tokenizer->At[0] - '0');
                        Number += Coefficient * Digit;
                        Coefficient *= 0.1f;
                        AdvanceInput(Tokenizer, 1);
                    }
                }
                
                Token.Type = Token_Literal;
                Token.Float = Number;
                Token.Integer = (s32)Number;
            } else {
                Token.Type = Token_Unknown;
            }
        } break;
    }

    ++Tokenizer->TokensCount;
    Token.Text.Count = (Tokenizer->Input.Data - Token.Text.Data);
    return Token;
}

token PeekToken(tokenizer* Tokenizer, int PeekCount) {
    token Result = {};

    while (PeekCount--) {
        Result = GetToken(Tokenizer);
    }
    return Result;
}

token PeekAheadToken(tokenizer* Tokenizer) {
    tokenizer Temp = *Tokenizer;
    token Result = GetToken(Tokenizer);
    *Tokenizer = Temp;
    return Result;
}

token PeekTokenSkipSpace(tokenizer* Tokenizer) {
    token Token;
    u32 Parsing = true;
    while (Parsing) {
        Token = GetToken(Tokenizer);
        switch (Token.Type) {
            case Token_EndOfLine:
            case Token_Unknown:
            case Token_Space: {
                printf("");
            } break;

            default: {
                Parsing = false;
            } break;
        }
    }
    
    return Token;
}