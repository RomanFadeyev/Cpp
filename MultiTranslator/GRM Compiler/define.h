#if !defined(AFX_DEFINE_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_DEFINE_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#define ALPHABET_SIZE		128

#define EXIT_TRUE		-1
#define EXIT_FALSE		-2


#define NO_INDEX	(int)(-1)

#define EMPTY_ELEMENT	1.0f	// обязательно должен типа double



////-------------------- Errors -------------------------

// %s - строка Lexeme.Name
// %t - тип Lexeme.pType->pLexeme

#define E_SYNTAX_ERROR					"Syntax error"
#define E_DUPLICATED_IDENTIFIER			"Duplicated identifier %s"

#define E_MISSING_SEMICOLON				"Misplaced ';'"
#define E_MISSING_COMA					"Misplaced ','"
#define E_MISSING_LEFT_BRACE			"Misplaced '{'"	
#define E_MISSING_RIGHT_BRACE			"Misplaced '}'"	
#define E_MISSING_LEFT_BRACKET			"Misplaced '('"
#define E_MISSING_RIGHT_BRACKET			"Misplaced ')'"
#define E_MISSING_LEFT_SBRACKET			"Misplaced '['"
#define E_MISSING_RIGHT_SBRACKET		"Misplaced ']'"

#define E_DECLARATION_INCORRECTLY		"Declaration is incorrect"
#define E_NOT_ACCESSIBLE_ASSIGNMENT		"Not accessible assignment"
#define E_UNSIMPLE_ASSIGNMENT			"Assignment %s to composit type "
#define E_UNSTRUCTURE_VARIABLE			"Structure variable is needed"
#define E_NEED_CONST_EXP				"Const expression is needed"
#define E_NULL_SIZE_OF_ARRAY			"Null size of array"
#define E_MISSING_RULE_WORD				"Missing word 'rule'"
#define E_MISSING_VARIANT_WORD			"Missing word 'variant'"
#define E_MISSING_TERM_WORD				"Missing word 'term' or 'symbol'"
#define E_INCORRECT_ON_OFF_A_EXPRESSION	"LexFunctions must be only 'on', 'off' or 'auto' "
#define E_BAD_EXPRESSION				"Bad expression"
#define E_MISSING_LIBRARY_WORD			"Missing word 'library'"

#endif // !defined(AFX_DEFINE_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_)
