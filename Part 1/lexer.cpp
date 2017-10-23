//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 1: Lexical Analyzer
// created: 02-01-17
// author:  Joshua Church
//*****************************************************************************

// Header files
#include "lexer.h"
#include <ctype.h>
#include <assert.h>
#include <string.h>

//*****************************************************************************
// Do the lexical parsing
char lexeme[MAX_LEXEME_LEN];    // Character buffer for lexeme
int token = -1;                 // Stores the current lexeme's token value
char cache = -1;                // Stores the last character read in from the input file.

// Functions within lexer.cpp
int checkChar(char);            // Handles the char when yylex() is called
int indentifier(char);          // Handles identifier lexeme values
int digit(char);                // Handles integer and float values
int strings(char);              // Handles string literals
int checkNext(char);            // Handles lexemes that change according the next char input
int setAndFetchNext(char);      // Stores the current char and fetches the next char in the file
int concealed(char c);          // Handles comments, whitespace, and newline characters
int checkLength();              // Verifies that the current lexeme does not exceed the max value.

// This function determines if the current lexeme has exceeded the maximum allocated length.
int checkLength() {

    // Verify that the current lexeme does not exceed the max value. 
    if (yyleng < MAX_LEXEME_LEN - 1)
        return true;
    
    // If the max value has exceeded the limit, alert to screen and update token.
    else {
        printf("ERROR: This lexeme has exceeded the maximum character length.\n");
        return false;
    }
}

// This function sets the current char into the lexeme buffer and fetches the next file char.
int setAndFetchNext(char c) {
    yytext[yyleng++] = c;
    c = fgetc(yyin);
    return c;
}

// This function determines the action of the current char value.
// This function is called whenever yylex() is invoked, so it is the lead char
// of the current lexeme.
int checkChar(char c) {
    
    // If the char is [a-z][A-Z], pass it to the identifier function.
    if (isalpha(c)) 
        token = indentifier(c);

    // If the char is [0-9], pass it to the digit function.
    else if (isdigit(c)) 
        token = digit(c);

    // If the char is a double quote ["], pass it to the strings function.
    else if (c == '"')
        token = strings(c);

    // If the char value can change according to the next char, pass it to the checkNext function.
    // [:] --> [:=] OR [=] --> [==] OR [<] --> [<>] OR [$] --> [$alpha...]
    else if (c == ':' || c == '=' || c == '<' || c == '$')
        token = checkNext(c);

    // Otherwise, the current char should be checked against the other independent defined tokens.
    else {
        switch (c) {
                
            // Check if char is a semicolon
            case ';':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_SEMICOLON;
                break;

            // Check if char is an opening parenthesis
            case '(':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_OPENPAREN;
                break;
            
            // Check if char is a closing parenthesis
            case ')':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_CLOSEPAREN;
                break;

            // Check if char is an opening bracket
            case '[':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_OPENBRACKET;
                break;

            // Check if char is a closing bracket
            case ']':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_CLOSEBRACKET;
                break;
                
            // Check if char is an opening brace
            case '{':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_OPENBRACE;
                break;
                
            // Check if char is a closing brace
            case '}':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_CLOSEBRACE;
                break;

            // Check if char is a comma
            case ',':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_COMMA;
                break;

            // Check if char is a dot
            case '.':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_DOT;
                break;

            // Check if char is a backslash slash
            case '\\':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_SLASH;
                break;

            // Check if char is a plus operator
            case '+':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_PLUS;
                break;

            // Check if char is a minus operator
            case '-':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_MINUS;
                break;

            // Check if char is a multiply operator
            case '*':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_MULTIPLY;
                break;

            // Check if char is a divide operator
            case '/':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_DIVIDE;
                break;

            // Check if char is a greather than operator
            case '>':
                c = setAndFetchNext(c);
                cache = c;
                token = TOK_GREATERTHAN;
                break;

            // If the char does not match any of the following cases,
            // then the char is a unknown character (undefined as a token)
            default:
                c = setAndFetchNext(c);
                cache = c;  
                token = TOK_UNKNOWN;
                break;
        }
    }
    
    // Return the current token back to yylex()
    return token;
}

// This function checks whether the current lexeme is
// an identifier, keyword, data type specifier, or operator keyword
int indentifier(char c) {
    
    // Store current char and fetch the next char.
    c = setAndFetchNext(c);
    
    // Continue to fetch and store characters until an invalid indentifier character is reached
    // This means that if the char is not an alpha [a-z][A-Z] OR digit [0-9] OR underscore [_],
    // then the current lexeme must stop taking characters
    while (isalpha(c) || isdigit(c) || c == '_')
        c = setAndFetchNext(c);

    // Store the last char in the cache to utilize later
    cache = c;

    // If the current lexeme exceeds the max length, then it is unknown.
    if (!checkLength())
        return TOK_UNKNOWN;

    // If the current lexeme matches the datatype keyword "int", return proper token
    else if (strcmp(yytext, "int") == 0)
        return TOK_INT;

    // If the current lexeme matches the datatype keyword "float", return proper token
    else if (strcmp(yytext, "float") == 0) 
        return TOK_FLOAT;

    // If the current lexeme matches the datatype keyword "string", return proper token
    else if (strcmp(yytext, "string") == 0)
        return TOK_STRING;
    
    // If the current lexeme matches the operator keyword "and", return proper token
    else if (strcmp(yytext, "and") == 0)
        return TOK_AND;

    // If the current lexeme matches the operator keyword "or", return proper token
    else if (strcmp(yytext, "or") == 0)
        return TOK_OR;

    // If the current lexeme matches the operator keyword "not", return proper token
    else if (strcmp(yytext, "not") == 0)
        return TOK_NOT;

    // If the current lexeme matches the operator keyword "length", return proper token
    else if (strcmp(yytext, "length") == 0)
        return TOK_LENGTH;

    // If the current lexeme matches the keyword "combinator", return proper token
    else if (strcmp(yytext, "combinator") == 0)
        return TOK_COMBINATOR;

    // If the current lexeme matches the keyword "evaluate", return proper token
    else if (strcmp(yytext, "evaluate") == 0)
        return TOK_EVALUATE;

    // If the current lexeme matches the keyword "dictionary", return proper token
    else if (strcmp(yytext, "dictionary") == 0)
        return TOK_DICTIONARY;

    // If the current lexeme matches the keyword "set", return proper token
    else if (strcmp(yytext, "set") == 0)
        return TOK_SET;

    // If no keywords match, the current lexeme is an identifier
    else 
        return TOK_IDENTIFIER;
}


// This function checks if the following lexeme is an
// integer or float value
int digit(char c) {
    
    // Continue fetching chars until a non-digit char is obtained.
    while (isdigit(c))
        c = setAndFetchNext(c);

    // Store the last char in the cache to utilize later
    cache = c;
    
    // If the current lexeme exceeds the max length, then it is unknown.
    if (!checkLength())
        return TOK_UNKNOWN;
    
    // If a "dot" value [.] is found, then
    // the current lexeme is a float
    if (c == '.') {
        
        // Store current char and fetch the next char.
        c = setAndFetchNext(c);
        
        // Continue fetching chars until a non-digit char is found
        while (isdigit(c))
            c = setAndFetchNext(c);

        // Store the last char is the cache to utilize later
        cache = c;
        
        // If the current lexeme exceeds the max length, then it is unknown.
        if (!checkLength())
            return TOK_UNKNOWN;
        
        // Otherwise, the current lexeme is a float
        return TOK_FLOATLIT;

    }
    
    // Current lexeme is an integer literal
    return TOK_INTLIT;
}

// This function handles string lexemes
int strings(char c) {
    
    // Store current char and fetch the next
    c = setAndFetchNext(c);

    // Continue to fetch characters until another ["] (or EOF) is found
    while (c != '"') {
        
        // While not EOF, continue to fetch next char
        if (!feof(yyin)) {
            
            // Store current char and fetch the next
            c = setAndFetchNext(c);
        }
        
        // If EOF is met before another ["], set the token appropriately and return
        else {
            
            // Store the last char in the cache to utilize later
            cache = c;
            
            // Return EOF token during string literal
            return TOK_EOF_SL;
        }
    }

    // Now that a ["] has been found, store it and fetch the next char
    c = setAndFetchNext(c);
    
    // Store the last char in the cache to utilize later
    cache = c;
    
    // If the current lexeme exceeds the max length, then it is unknown.
    if (!checkLength())
        return TOK_UNKNOWN;
    
    // Otherwise, the current lexeme is a string literal
    return TOK_STRINGLIT;
}

// The following function handles a char that can change according to the next char
// [:] --> [:=] OR [=] --> [==] OR [<] --> [<>] OR [$] --> [$alpha...]
int checkNext(char c) {
    
    // This handles c-identifiers
    if (c == '$') {
        
        // Store current and fetch next
        c = setAndFetchNext(c);
        
        // If the next char is not [a-z][A-Z], then it is unknown
        if (!isalpha(c)) {
            
            // Store the last char in cache to utilize later
            cache = c;
            
            return TOK_UNKNOWN;
        }
        
        // Otherwise, we continue to get the next char until
        // a non-alpha, non-digit, non-underscore char is found
        else {
            while (isalpha(c) || isdigit(c) || c == '_')
                c = setAndFetchNext(c);
            
            // Store the last char in the cache to utilize later
            cache = c;
            
            // If the current lexeme exceeds the max length, then it is unknown.
            if (!checkLength())
                return TOK_UNKNOWN;
            
            // Otherwise, it is a c-identifier
            return TOK_CIDENTIFIER;
        }
    }

    // This is a check for the assign lexeme
    else if (c == ':') {
        
        // Store current and fetch next
        c = setAndFetchNext(c);
        
        // If the next char is '-', then the lexeme is now the assign lexeme
        if (c == '=') {
            
            // Store current char and fetch next
            c = setAndFetchNext(c);
            
            // Store the last char in the cache to utilize later
            cache = c;
            
            // Return appropropriate token
            return TOK_ASSIGN;
        }
        
        // Otherwise, the current lexeme is unknown
        else {
            
            // Store the last char in the cache to utilize later
            cache = c;
            
            // Return unknown token
            return TOK_UNKNOWN;
        }
    }

    // This is a check for the not equal to lexeme
    else if (c == '<') {
        
        // Store current and fetch next
        c = setAndFetchNext(c);
        
        // If the next char is not the '>' char (greater than)
        // Then the lexeme token is the set to the less than token
        if (c == '>') {
            c = setAndFetchNext(c);
            cache = c;
            return TOK_NOTEQUALTO;
        }
        else {
            // Store the last char in the cache to utilize later
            cache = c;
            
            // Return less than token
            return TOK_LESSTHAN;
        }
    }

    // This is a check for the equality lexeme
    else if (c == '=') {
        c = setAndFetchNext(c);
        
        // If the next char is not the '=' symbol, then the
        // current lexeme is unknown.
        if (c == '=') {
            c = setAndFetchNext(c);
            cache = c;
            return TOK_EQUALTO;
        }
        else {
            // Store the last char in the cache to utilize later
            cache = c;
            
            // Return unknown token
            return TOK_UNKNOWN;
        }

    }
}

// The following function handles
// comments [#], whitespace [ ], and newlines [\n]
int concealed(char c) {
    
    while (c == '#' || c == ' ' || c == '\n') {
        
        // Continue to fetch characters until
        // a newline character is found
        if (c == '#') {
            while (c != '\n')
                c = fgetc(yyin);
            
            // Increment the line count
            yyLine++;
            
            // Fetch the next char
            c = fgetc(yyin);
        }
        
        // Continue fetching characters until a non-space is found
        if (c == ' ') {
            while (c == ' ') {
                c = fgetc(yyin);
            }
        }
        
        // Continue fetching newline characters
        if (c == '\n') {
            while (c == '\n') {
                
                // Update the line count whenever
                // a newline char is found
                yyLine++;
                c = fgetc(yyin);
            }
        }
    }
    
    return c;
}

// Main function that handles the lexical parsing
int yylex() {

    // The starting char is the last known char stored in the cache
    char c = cache;

    // Prepare the buffer to hold the lexeme
    for(int i = 0; i < MAX_LEXEME_LEN; i++)
        lexeme[i] = '\0';

    // Reset back to default values
    yytext = lexeme;
    yyleng = 0;

    // Fetch first char
    if (c < 0) 
        c = fgetc(yyin);

    // Handles comments [#], whitespace [ ], and newlines [\n]
    c = concealed(c);

    // If EOF, return proper token to driver
    if(feof(yyin)) {
        c = -1;
        return(TOK_EOF);
    }

    // Return token back to driver
    return checkChar(c);
}
