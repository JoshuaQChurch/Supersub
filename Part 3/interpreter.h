#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parseTree.h"

// Change all unbound variables to capitalized variables and 
// rename all bound variables based on depth of the binding scope
// so that substititions can be performed without variable capture
expressionP canonicalRenameBoundVars(expressionP e, int &count) ;

// Remove any repeats 
expressionP canonicalRemoveRepeats(expressionP e, int &count);

// Return true of expression has unbounded variables (only works after calling
// canonicalRenameBoundVars that converts unbound variables to uppercase)
bool hasUnboundVars(expressionP e) ;

//-----------------------------------------------------------------------------
// Recursive pre-order search for APPLICATION node to substitute and perform
// substitution.
// Input: e is the input expression that will be transformed
// Input: dictionary of combinators
// Input: count for unique variable names
// Output: found boolean set to true when substitution has been found (and 
//         executed
// Output: returns new substitution expression, input expression is consumed
//-----------------------------------------------------------------------------
expressionP 
preOrderSubstitutionSearch(expressionP e,
			   const expressionDictionary &dictionary,
			   int &count, 
			   bool & found) ;

// Perform at most evalDepth steps of pre-order evaluations of input expression
// e.  If printLevel is nonzero, then print progress of evaluation.
expressionP
evaluatePreOrder(expressionP e, const expressionDictionary &dictionary,
		 int evalDepth, int printLevel) ;

//-----------------------------------------------------------------------------
// Recursive post-order search for APPLICATION node to substitute and perform
// substitution.
// Input: e is the input expression that will be transformed
// Input: dictionary of combinators
// Input: count for unique variable names
// Output: found boolean set to true when substitution has been found (and 
//         executed
// Output: returns new substitution expression, input expression is consumed
//-----------------------------------------------------------------------------
expressionP 
postOrderSubstitutionSearch(expressionP e,
			    const expressionDictionary &dictionary,
			    int &count, 
			    bool & found) ;

// Perform at most maxNumSteps of post-order evaluations of input 
// expression e.  If printLevel is nonzero, then print progress of
// evaluation.
expressionP
evaluatePostOrder(expressionP e, const expressionDictionary &dictionary,
		  int maxNumSteps, int printlevel) ;
#endif
