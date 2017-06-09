#pragma once
enum ECodeOperation{    NOP,            PUSH,       POP,        EQ, 
                        INEQ,           LESS,       MORE,       LESS_EQ,
                        MORE_EQ,        SHL,        SHR,        ADD, 
                        SUB,            MUL,        DIV,        RMNDR, 
                        GET_JZ,         JMP,        NEW,        ASSIGN,
                        NEW_LEVEL,      EXIT_LEVEL, RET,
                        STORE,          RESTORE,    UNSTORE,    ENDF,
                        PUSH_EMPTY,     CALL,       FUNC,       PRINT,
                        NEWA,           POP_NEW,    SAVE,       LOAD,
                        STORE_IDX,      HALT,       LABEL,      END_ISEGM,
                        INC_POST,       DEC_POST,   POP_ALIAS,  SIZEOF,
                        SIZEA,          SUBROUTINE, 
                        PRMCOUNT,       PRMSTR,     ENDSUBR,    PARSE,
                        RET_S,          NOT,        INVERT,     NEG,//INVERT не используется
                        L_AND,          L_OR,       AND,        OR,
                        XOR,            WRITE,      GET_TEXT,   GET_FILE_POS,
                        INS_BEFORE,     INS_AFTER,  
						//Настройки 
						S_WORDS,		I_W_SPACES, I_BLOCK,	I_CASE,

                        FORMAT,         REPLACE_BLOCK,  RECALL_EXTERN, SAVE_FILES,
                        RESERVED    
                    };

enum EAssignType{	SIMPLE_ASSIGN   =0,
					ADD_ASSIGN      =1,
					MUL_ASSIGN      =2,
					SUB_ASSIGN      =3,
					DIV_ASSIGN      =4,
					RMNDR_ASSIGN    =5,
					SHL_ASSIGN      =6,
					SHR_ASSIGN      =7,
					AND_ASSIGN      =8,
					XOR_ASSIGN      =9,
					OR_ASSIGN       =10};