string GetTokenTypeName(token_type Type) {
    switch (Type) {
        case Token_Unknown:
        default: { return make_string("Token_Unknown"); } break;

        case Token_OpenParen: { return make_string("Token_OpenParen"); } break;
        case Token_CloseParen: { return make_string("Token_CloseParen"); } break;
        case Token_Colon: { return make_string("Token_Colon"); } break;
        case Token_Semicolen: { return make_string("Token_Semicolen"); } break;
        case Token_Asterisk: { return make_string("Token_Asterisk"); } break;
        case Token_OpenBracket: { return make_string("Token_OpenBracket"); } break;
        case Token_CloseBracket: { return make_string("Token_CloseBracket"); } break;
        case Token_OpenBrace: { return make_string("Token_OpenBrace"); } break;
        case Token_CloseBrace: { return make_string("Token_CloseBrace"); } break;
        case Token_Equals: { return make_string("Token_Equals"); } break;
        case Token_Comma: { return make_string("Token_Comma"); } break;
        case Token_Or: { return make_string("Token_Or"); } break;
        case Token_Pound: { return make_string("Token_Pound"); } break;
        case Token_Underscore: { return make_string("Token_Underscore"); } break;
        case Token_Tilde: { return make_string("Token_Tilde"); } break;

        case Token_String: { return make_string("Token_String"); } break;
        case Token_Identifier: { return make_string("Token_Identifier"); } break;
        case Token_Literal: { return make_string("Token_Literal"); } break;

        case Token_Space: { return make_string("Token_Space"); } break;
        case Token_EndOfLine: { return make_string("Token_EndOfLine"); } break;
        case Token_Comment: { return make_string("Token_Comment"); } break;

        case Token_EndOfStream: { return make_string("Token_EndOfStream"); } break;
    }
}

b32 IsTokenValid(token Token) {
    b32 Result = (Token.Type != Token_Unknown);
    return Result;
}

b32 TokenEquals(token Token, char* Match) {
    b32 Result = StringsMatch((char*)Token.Text.Data, Match);
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
        Tokenizer->At[0] = Tokenizer->Input.Data[0];
        Tokenizer->At[1] = Tokenizer->Input.Data[1];
    }
}

void AdvanceChars(tokenizer* Tokenizer, u32 Count) {
    Tokenizer->ColumnCount += Count;
    Advance(&Tokenizer->Input, Count);
    Refill(Tokenizer);
}

tokenizer Tokenize(string Data, char* Filename) {
    tokenizer Result = {};
    
    Result.Filename = Filename;
    Result.LineCount = 1;
    Result.TokenCount = 1;   
    Result.ColumnCount = 1;
    Result.Input = Data;
    Refill(&Result);
    
    return(Result);
}

token GetTokenRaw(tokenizer* Tokenizer) {
	token Token = {};
    Token.Filename = Tokenizer->Filename;
	Token.Text = Tokenizer->Input;

	char C = Tokenizer->At[0];
    AdvanceChars(Tokenizer, 1);
	switch (C) {
        case '\0': { Token.Type = Token_EndOfStream; } break;

        case '(': { Token.Type = Token_OpenParen; } break;
        case ')': { Token.Type = Token_CloseParen; } break;
        case ':': { Token.Type = Token_Colon; } break;
        case ';': { Token.Type = Token_Semicolen; } break;
        case '*': { Token.Type = Token_Asterisk; } break;
        case '[': { Token.Type = Token_OpenBracket; } break;
        case ']': { Token.Type = Token_CloseBracket; } break;
        case '{': { Token.Type = Token_OpenBrace; } break;
        case '}': { Token.Type = Token_CloseBrace; } break;
        case '=': { Token.Type = Token_Equals; } break;
        case ',': { Token.Type = Token_Comma; } break;
        case '|': { Token.Type = Token_Or; } break;
        case '#': { Token.Type = Token_Pound; } break;
        case '_': { Token.Type = Token_Underscore; } break;
        case '~': { Token.Type = Token_Tilde; } break;

        case '"': { // note(jax): We've got a string
			Token.Type = Token_String;
            while(Tokenizer->At[0] && Tokenizer->At[0] != '"') {
                if ((Tokenizer->At[0] == '\\') && (Tokenizer->At[1])) {
                    AdvanceChars(Tokenizer, 1);
                }

                AdvanceChars(Tokenizer, 1);
            }

            if (Tokenizer->At[0] == '"') {
                AdvanceChars(Tokenizer, 1);
            }
        } break;

        default: {
            if (IsSpacing(C)) {
                Token.Type = Token_Space;
                while (IsSpacing(Tokenizer->At[0])) {
                    AdvanceChars(Tokenizer, 1);
                }
            } else if (IsEndOfLine(C)) {
                Token.Type =  Token_EndOfLine;
                if(((C == '\r') &&
                    (Tokenizer->At[0] == '\n')) ||
                   ((C == '\n') &&
                    (Tokenizer->At[0] == '\r'))) {
                    AdvanceChars(Tokenizer, 1);
                }
                
                ++Tokenizer->LineCount;
            } else if((C == '/') && (Tokenizer->At[0] == '/')) {
                Token.Type = Token_Comment;
                
                AdvanceChars(Tokenizer, 2);
                while(Tokenizer->At[0] && !IsEndOfLine(Tokenizer->At[0])) {
                    AdvanceChars(Tokenizer, 1);
                }
            } else if((C == '/') &&
                    (Tokenizer->At[0] == '*')) {
                Token.Type = Token_Comment;
                
                AdvanceChars(Tokenizer, 2);
                while(Tokenizer->At[0] && !((Tokenizer->At[0] == '*') 
                    && (Tokenizer->At[1] == '/'))) {
                    if(((Tokenizer->At[0] == '\r') && (Tokenizer->At[1] == '\n')) 
                        || ((Tokenizer->At[0] == '\n') && (Tokenizer->At[1] == '\r'))) {
                        AdvanceChars(Tokenizer, 1);
                    }
                    
                    if (IsEndOfLine(Tokenizer->At[0])) {
                        ++Tokenizer->LineCount;
                    }
                    
                    AdvanceChars(Tokenizer, 1);
                }
                
                if(Tokenizer->At[0] == '*') {
                    AdvanceChars(Tokenizer, 2);
                }
            } else if (IsAlphabetical(C)) {
				Token.Type = Token_Identifier;
                while ((IsAlphabetical(Tokenizer->At[0]) || IsNumeric(Tokenizer->At[0]) || IsPunctuation(Tokenizer->At[0]) || !IsSymbolic(Tokenizer->At[0]) || Tokenizer->At[0] == ' ') // General grammar for Strings
                    && !IsEndOfLine(Tokenizer->At[0]) // We should end the loop if we rreach a new line
                    && (Tokenizer->At[0] != '"')) // Check if this char is a string
                {
                    AdvanceChars(Tokenizer, 1);
                }

                { // Sometimes we will have a Token.Text that has a space at the end, but only one 
                  // word. Therefore this is some hacky code to determine if there is > than one word.
                  // If there IS more than one word, the token is a string. We should probably find a 
                  // better way to do this!!!
                    char Buffer[256];
                    sprintf(Buffer, "%.*s", (u32)(Tokenizer->Input.Data - Token.Text.Data), Token.Text.Data);

                    char* Temp;
                    if (Temp = Substring(Buffer, " ")) {
                        if (StringLength(Temp) > 1)
                            Token.Type = Token_String;
                    }
                }
            } else if (IsNumeric(C)) {
                f32 Number = (f32)(C - '0');
                
                while (IsNumeric(Tokenizer->At[0])) {
                    f32 Digit = (f32)(Tokenizer->At[0] - '0');
                    Number = 10.0f*Number + Digit;
                    AdvanceChars(Tokenizer, 1);
                }
                
                // This float detection code is copy pasted, I have no idea how it works!
                if (Tokenizer->At[0] == '.') {
                    AdvanceChars(Tokenizer, 1);
                    f32 Coefficient = 0.1f;
                    while (IsNumeric(Tokenizer->At[0])) {
                        f32 Digit = (f32)(Tokenizer->At[0] - '0');
                        Number += Coefficient * Digit;
                        Coefficient *= 0.1f;
                        AdvanceChars(Tokenizer, 1);
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

    ++Tokenizer->TokenCount;
    Token.Text.Count = (Tokenizer->Input.Data - Token.Text.Data);
    return Token;
}

token PeekTokenRaw(tokenizer *Tokenizer) {
    tokenizer Temp = *Tokenizer;
    token Result = GetTokenRaw(Tokenizer);
    *Tokenizer = Temp;
    return Result;
}

token GetToken(tokenizer* Tokenizer) {
    token Token;
    for (;;) {
        Token = GetTokenRaw(Tokenizer);
        if((Token.Type == Token_Space) ||
           (Token.Type == Token_EndOfLine) ||
           (Token.Type == Token_Comment)) {
            // note(jax): Ignore these when we're getting "real" tokens
        } else {
            if(Token.Type == Token_String) {
                if(Token.Text.Count &&
                   (Token.Text.Data[0] == '"')) {
                    ++Token.Text.Data;
                    --Token.Text.Count;
                }
                
                if(Token.Text.Count &&
                   (Token.Text.Data[Token.Text.Count - 1] == '"')) {
                    --Token.Text.Count;
                }
            }
            
            break;
        }
    }

    return Token;
}

token PeekToken(tokenizer* Tokenizer, int PeekCount) {
    token Result = {};

    while (PeekCount--) {
        Result = GetToken(Tokenizer);
    }
    return Result;
}

token PeekToken(tokenizer* Tokenizer) {
    tokenizer Temp = *Tokenizer;
    token Result = GetToken(&Temp);
    return Result;
}

token PeekTokenSkipSpace(tokenizer* Tokenizer) {
    token Token;
    u32 Parsing = true;
    while (Parsing) {
        Token = GetToken(Tokenizer);
        switch (Token.Type) {
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