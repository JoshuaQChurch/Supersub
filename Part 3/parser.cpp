//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 3
// assignment: Recursive Decent Parser
// name: Joshua Church
// netID: jqc10
//*****************************************************************************
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>

using namespace std;

#include "lexer.h"
#include "parseTree.h"
#include "interpreter.h"

//#define VERBOSE
expressionDictionary combinatorDictionary ;

// Production functions
void P(void);
void S(void);
void J(void);
void K(void);
void L(void);
void M(void);

// Handle for production rules that return integers
int E(void);
int T(void);
int R(void);

// Handle for production rules that return expressions
expressionP A(void);
expressionP I(void);
expressionP F(void);

// Function declarations for checking whether the current token is
// in the first set of each production rule.
bool IsFirstOfP(void);
bool IsFirstOfS(void);
bool IsFirstOfJ(void);
bool IsFirstOfK(void);
bool IsFirstOfL(void);
bool IsFirstOfM(void);
bool IsFirstOfA(void);
bool IsFirstOfF(void);
bool IsFirstOfI(void);
bool IsFirstOfE(void);
bool IsFirstOfT(void);
bool IsFirstOfR(void);

//*****************************************************************************
// Function to help print the tree nesting
string psp( int n ) // Stands for p-space, but I want the name short
{
    string str( n, ' ' );
    return str;
}

// Needed global variables
int iTok;                   // The current token
static int maxEvalSteps = 10000;   // Sets max number of substitution steps in evaluation
static int preOrderEvaluate = 1;   // Non-zero means use pre-order evaluation, otherwise post-order
static int printLevel = 1;         // Non-zero means print each step during evaluation

//*****************************************************************************
// The main processing loop
int main (int argc, char* argv[]) {
    int token;   // hold each token code

    // Set the input and output streams
    yyout = stdout;
    yyin = stdin;
    // Get the first token
    iTok = yylex();
    // Fire up the parser!
    try {
        // P production rule
        P();
        
        if(iTok != TOK_EOF)
            throw "Invalid Program Statement";
    }
    catch( char const *errmsg ) {
        cout << endl << "***ERROR (line " << yyLine << "): "<< errmsg << endl;
        //exit(-1) ;
        while(!feof(yyin)) {
            iTok = yylex() ;
            if(iTok==TOK_SEMICOLON)
                break ;
        }
        iTok = yylex() ;
    }

    // Upon completion, settings and the overall combinator dictionary
    cout << "Combinator Dictionary is:" << endl ;
    combinatorDictionary.Print(cout);
    cout << "variableSettings:" << endl;
    cout << "maxEvalSteps: " << maxEvalSteps << endl;
    cout << "preOrderEvaluate: " << preOrderEvaluate << endl;
    cout << "printLevel: " << printLevel << endl;
    return 0;
}

//*****************************************************************************
// P --> { S }
void P (void) {
    
    static int Pcnt = 0; // Count the number of P's
    int CurPcnt = Pcnt++;
    char const *Perr =
        "statement does not start with recognized keyword";
    
    // Check the S production rule
    while (IsFirstOfS()) {
        S();
    }
}

// S --> ( J | K | L | M) ";"
void S(void) {
    
    static int Scnt = 0; // Count the number of S's
    int CurScnt = Scnt++;
    char const *Serr =
        "statement does not end with a ';'";
    
    switch (iTok) {

    	// If the "combinator" token is found, descend into the J production rule
        case TOK_COMBINATOR:
            J();
            break;
        
        // If the "evaluate" token is found, descend into the K production rule
        case TOK_EVALUATE:
            K();
            break;
            
        // If the "dictionary" token is found, descend into the L production rule
        case TOK_DICTIONARY:
            L();
            break;
            
        // If the "set" token is found, descend into the M production rule
        case TOK_SET:
            M();
            break;
    }
    
    
    // A semicolon is expected here. 
    if (iTok == TOK_SEMICOLON) 
        iTok = yylex();
    else  
        throw Serr;

}

// J --> "combinator" ["evaluate"] id A
void J(void) {

    static int Jcnt = 0; // Count the number of J's
    int CurJcnt = Jcnt++;
    expressionP term;
    string item;
    
    iTok = yylex();

    bool evaluate = (iTok == TOK_EVALUATE);
    
    // Optional evaluate token 
    if (evaluate) {
        iTok = yylex();
    }
    
    // Expect an identifier
    if (iTok == TOK_IDENTIFIER) {
        
        // Store current lexeme to insert into dictionary
        item = yytext;
        iTok = yylex();
    }
    
    else 
        throw "missing Identifier";

    
    if (IsFirstOfA()) {
        int count = 0;
        // Perform the A() production rule and rename any
        // bound variables
        term = A();
        term = canonicalRenameBoundVars(term, count);
        
        // If there is an unbound variable, throw a flag.
        if (hasUnboundVars(term))
            throw "Not a valid combinator";
        
        else {
            // Only evaluate if we receive the evaluate token.
            if (evaluate) {
                
                // Evaluate pre-order
                if (preOrderEvaluate != 0)
                    term = evaluatePreOrder(term, combinatorDictionary, maxEvalSteps, 0);
                
                // Evaluate post-order
                else
                    term = evaluatePostOrder(term, combinatorDictionary, maxEvalSteps, 0);
            }
        }
        
        // Add to the dictionary for later usage.
        combinatorDictionary.insertItem(item, term);
    }
    else 
        throw "missing 'A' production";
}

// K --> "evaluate" A
void K(void) {

    static int Kcnt = 0; // Count the number of K's
    int CurKcnt = Kcnt++;
    expressionP term;
    
    iTok = yylex();
    
    if (IsFirstOfA())
        term = A();
    else
        throw "expected 'A' production rule, but known found.";
    
    // Determines whether we follow pre or post-order
    if (preOrderEvaluate != 0)
        term = evaluatePreOrder(term, combinatorDictionary, maxEvalSteps, printLevel);
    
    else
        term = evaluatePostOrder(term, combinatorDictionary, maxEvalSteps, printLevel);

    cout << "Expression Evaluates To: " << term->String() << endl;
}

// L --> "dictionary"
void L(void) {
    static int Lcnt = 0; // Count the number of L's
    int CurLcnt = Lcnt++;
    expressionP term;
    
    cout << "Combinator Dictionary is:" << endl ;
    combinatorDictionary.Print(cout);
    iTok = yylex();
}

// M --> "set" id E
void M(void) {

    static int Mcnt = 0; // Count the number of M's
    int CurMcnt = Mcnt++;
    int term;
    string id_name;
    iTok = yylex();
    
    // Expect to get an identifier
    if (iTok == TOK_IDENTIFIER) {

        // Store current lexeme for printing. 
        id_name = yytext;
        iTok = yylex();
    }
    
    else 
        throw "expecting Identifier.";
    
    if (IsFirstOfE()) 
        term = E();
    else 
        throw "missing E production rule";
    
    // Handle to change maximum allowed steps
    if (id_name == "maxEvalSteps")
        maxEvalSteps = term;
    
    // Handle to change pre vs post-order
    else if (id_name == "preOrderEvaluate")
        preOrderEvaluate = term;
    
    // Handle to allow printing at each level of execution
    else if (id_name == "printLevel")
        printLevel = term;

}

// A --> I {I}
expressionP A(void) {

    static int Acnt = 0; // Count the number of A's
    int CurAcnt = Acnt++;
    char const *Aerr =
    "expected 'I' production rule, but known found.";
    
    expressionP term;

    // We expect an I here.
    if (IsFirstOfI()) {
        term = I();
    }
    
    else
        throw Aerr;
    
    // The I might be followed by a series of I's.
    while (IsFirstOfI()) {
        term = makeApplication(term, I());
    }
    
    return term;
}

// F --> "\" ID "." A | "{" F "}"
expressionP F(void) {
    
    static int Fcnt = 0; // Count the number of F's
    int CurFcnt = Fcnt++;
    expressionP term;
    string id_name;
    
    // Expect to get '\'
    if (iTok == TOK_SLASH) {
        
        iTok = yylex();

        // Expect to get an identifier
        if (iTok == TOK_IDENTIFIER) {

            // Store current lexeme for later
            id_name = yytext;
            iTok = yylex();

            // Expect to get a '.'
            if (iTok == TOK_DOT) {

                iTok = yylex();
                
                if (IsFirstOfA()) 
                    term = makeSlashDot(id_name, A());
                else 
                    throw "F expecting expression after variable";
            }
            else 
                throw "expecting '.' in substitution expression";
        }
        else 
            throw "expecting ID";
    }
    

    // Expect to get '('
    else if (iTok == TOK_OPENBRACE) {

        iTok = yylex();
        
        if (IsFirstOfF()) 
            term = F();
        else 
            throw "expecting slash to lead substitution definition";
        
        // Expect to get ')'
        if (iTok != TOK_CLOSEBRACE) 
            throw "missing closing brace";

        iTok = yylex();
    }

    return term;

}

// I --> CID | id | "(" A ")" | F
expressionP I(void) {

    static int Icnt = 0; // Count the number of I's
    int CurIcnt = Icnt++;
    char const *Ierr =
    "factor does not start with 'CID' | 'ID' | '(' | F";
    expressionP term;

    // Expect to get an ID
    if (iTok == TOK_IDENTIFIER) {
        term = makeVariable(yytext);
        iTok = yylex();
    }
    
    // Expect to get a CID
    else if (iTok == TOK_CIDENTIFIER) {
        // Make a temp string to remove excess
        // dollar sign from the CID. makeCombinator
        // adds one to it, so this helps remove the
        // double dollar sign
        string dollarSign = yytext;
        term = makeCombinator(dollarSign.substr(1));
        iTok = yylex();
    }
    
    // Expect to get a '('
    else if (iTok == TOK_OPENPAREN) {

        iTok = yylex();

        if (IsFirstOfA()) {
            term = A();
        }
        else 
            throw "doesn't meet 'A' production rule";
      
      	// Expect to get a ')'
        if (iTok == TOK_CLOSEPAREN) {
            iTok = yylex();
        }
        else 
            throw "no closing parenthesis";
    }
    
    else if (IsFirstOfF()) 
        term = F();

    else 
        throw Ierr;

    return term;
}

// E --> T { ( + | - ) T }
int E (void) {
    
    int rValue1 = 0;   // The value to return
    static int Ecnt = 0; // Count the number of E's
    int CurEcnt = Ecnt++;
    char const *Terr =
    "term does not start with 'INTLIT' | '('";
    
    // We next expect to see a T
    if(IsFirstOfT())
        rValue1 = T();
    else
        throw Terr;
    
    // As long as the next token is + or -, keep parsing T's
    while (iTok == TOK_PLUS || iTok == TOK_MINUS) {

        int iTokLast = iTok;
        iTok = yylex();
        
        // Perform the operation to update rValue1 according to T()
        switch(iTokLast) {
            case TOK_PLUS:
                rValue1 = rValue1 + T();
                break;
                
            case TOK_MINUS:
                rValue1 = rValue1 - T();
                break;
        }
    }
    
    return rValue1;
}

// T --> R { ( * | / ) R }
int T(void) {
    
    int rValue1 = 0;   // The value to return
    static int Tcnt = 0; // Count the number of T's
    int CurTcnt = Tcnt++;
    char const *Rerr =
    "term does not start with 'INTLIT' | '('";

    // We next expect to see an R
    if(IsFirstOfR())
        rValue1 = R();
    else
        throw Rerr;
    
    // As long as the next token is + or -, keep parsing R's
    while (iTok == TOK_MULTIPLY || iTok == TOK_DIVIDE) {

        int iTokLast = iTok;
        iTok = yylex();
        
        // Perform the operation to update rValue1 according to R()
        switch(iTokLast) {
            case TOK_MULTIPLY:
                rValue1 = rValue1 * R();
                break;
                
            case TOK_DIVIDE:
                rValue1 = rValue1 / R();
                break;
            
            default:
                throw "expecting '*' or '/'";
        }
    }
    
    return rValue1;
}

// R --> INTLIT | (E)
int R(void) {

    int rValue = 0;
    static int Rcnt = 0; // Count the number of R's
    int CurRcnt = Rcnt++;
    
    switch (iTok) {

        case TOK_INTLIT:
            
            // Capture the value of this literal
            rValue = atoi(yytext);
            iTok = yylex();
            break;
            
        case TOK_OPENPAREN:
            // We expect (E); parse it
            
            iTok = yylex();
            rValue = E();
            if (iTok == TOK_CLOSEPAREN) {
                
                iTok = yylex();
            }
            else
                throw "expecting closing parenthesis";
            break;

        default:
            throw "factor does not start with 'INTLIT' | '('";
            break;
    }
    
    return rValue;
}


/*
	The following Boolean functions determine 
	if a production rule meets its criteria as 
	defined by the grammar.
*/

bool IsFirstOfP(void) {
    return IsFirstOfS();
}

bool IsFirstOfS(void) {
    return (IsFirstOfJ() || IsFirstOfK() || IsFirstOfL() || IsFirstOfM());
}

bool IsFirstOfJ(void) {
    return iTok == TOK_COMBINATOR;
}

bool IsFirstOfK(void) {
    return iTok == TOK_EVALUATE;
}

bool IsFirstOfL(void) {
    return iTok == TOK_DICTIONARY;
}

bool IsFirstOfM(void) {
    return iTok == TOK_SET;
}

bool IsFirstOfA(void) {
    return IsFirstOfI();
}

bool IsFirstOfF(void) {
    return iTok == TOK_SLASH || iTok == TOK_OPENBRACE;
}

bool IsFirstOfI(void) {
    return (iTok == TOK_CIDENTIFIER ||iTok == TOK_IDENTIFIER  ||
            iTok == TOK_OPENPAREN || IsFirstOfF());
}

bool IsFirstOfE(void) {
    return IsFirstOfT();
}

bool IsFirstOfT(void) {
    return IsFirstOfR();
}

bool IsFirstOfR(void) {
    return (iTok == TOK_INTLIT || iTok == TOK_OPENPAREN);
}
