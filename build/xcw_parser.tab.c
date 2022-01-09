/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:337  */

#define YYSTYPE void*
#define ToInt(k) ((int*)(k))
#define ToStr(k) ((string*)(k))

#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<vector>
#include<stack>
using namespace std;

void yyerror(const char *);
void yyerror(const string&);
extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int yylineno, charNum;

ostream &out = cout;       // 用于输出


struct Ptr_num{             // 用来传递参数，用IF_ptr_int表示传上来的是否为常量
    int ptr_int;
    string ptr_str;
    int IF_ptr_int;
    Ptr_num(int p_int){
        ptr_int = p_int;
        IF_ptr_int = 1; 
    }
    Ptr_num(string p_str){
        ptr_str = p_str;
        IF_ptr_int = 0; 
    }
    Ptr_num(){}
    void Print(){       //打印出数值，用于调试
        out << "-------------Ptr_print_in------------"<<endl;
        if(IF_ptr_int){
            out << "IF_ptr_int = " << IF_ptr_int << endl;
            out << ptr_int;
        }
        else{
            out << "IF_ptr_int = " << IF_ptr_int << endl;
            out << ptr_str;
        }
        out << endl;
        out << "-------------Ptr_print_out------------"<<endl;
    }
};

struct IDENT_scope{
    string IDENT_name;
    string IDENT_num;          // 变量的值可变，因此用string存储
    string IR_name;          // 在tigger中的变量名

    int Array_size;
    bool IDENT_if_array;       //是否为数组变量
    int Param_num;
    int Stack_loc;       //在函数中的位置
    int IF_Global;

    IDENT_scope(string name,string ir_name){       //常量的构造函数
        IDENT_name = name;
        IR_name = ir_name;
        IDENT_if_array = 0;    // 默认不是数组
    }

};

vector<IDENT_scope> Scope;     //符号表



IDENT_scope* find_define(string name){
    int i = Scope.size() - 1;
    if(i == -1)
        return nullptr;
    while(i >= 0){
        if(name == Scope[i].IDENT_name){
            return & Scope[i];
        }
        i--;
    }
    return nullptr;
}

int DEEP = 0;
int VAR_v_num = 0;

string IF_DEEP(){
    string str_if_deep = "";
    if(DEEP != 0)
        str_if_deep += "\t";
    return str_if_deep;
}

//-----------------函数语句打印相关变量------------------------------
vector<string> Func_Init;
vector<string> Func_Other;
string init_out;
string other_out;     // 这个string 用来记录其他的语句
void Out_Print(string s){
    if(s == "init"){
        for(int i = 0;i < Func_Init.size();i++){
            out << Func_Init[i] << endl;
        }
        Func_Init.clear();
        return;
    }
    if(s == "other"){
        for(int i = 0;i < Func_Other.size();i++){
            out << Func_Other[i] << endl;
        }
        Func_Other.clear();
        return;
    }
    // for(int i = 0;i < Func_Init.size();i++){
    //     out << Func_Init[i] << endl;
    // }
    // for(int i = 0;i < Func_Other.size();i++){
    //     out << Func_Other[i] << endl;
    // }
    //out << func_other_out << endl;
    // Func_Other.clear();
    // Func_Init.clear();
    // Func_Other.clear();
}

//----------------------变量初始化相关------------------
int Flag_init_in_func;            // 表示变量初始化是否在函数中，因为tigger只允许在函数中赋值
int Flag_def_out = 1;            // 表示变量是否为全局变量

int Flag_IF_nfunc = 0;
int Flag_f_init_nfunc = 0;  

//---------------------函数回填相关----------------
int Loc_Func_def;    //函数被定义的位置，用于最后的回填  
int Stack_Func_size;    //函数需要栈空间的大小
int Stack_Func_nparam;    //已经被占用的栈空间的大小

//=-------------------------------
int s_num = 1;    // 始终保留s0用于最后的返回

int VAR_a_num = 0;    // 函数参数寄存器


void IDENT_Assign(IDENT_scope* tmp1, string str2 ){
    // ass_num用于处理给数组赋值的情况
    if(tmp1->IF_Global == 0 && tmp1->IDENT_if_array == 0 ){ // 非数组的局部变量
        other_out = IF_DEEP() + "store " + str2 + " " + tmp1->IR_name;
        Func_Other.push_back(other_out);
    }
    else{
        other_out = IF_DEEP() + "loadaddr " + tmp1->IR_name + " s" + to_string(s_num);
        Func_Other.push_back(other_out);
        other_out = IF_DEEP() + "s" + to_string(s_num) + "[0] = " + str2;
        Func_Other.push_back(other_out);
        s_num ++;
    }
}


#line 234 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:337  */
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "xcw_parser.tab.h".  */
#ifndef YY_YY_HOME_XCW_XCW2_COMPILER_COMPILER_EEYORE2TIGGER_BUILD_XCW_PARSER_TAB_H_INCLUDED
# define YY_YY_HOME_XCW_XCW2_COMPILER_COMPILER_EEYORE2TIGGER_BUILD_XCW_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ADD = 258,
    SUB = 259,
    MUL = 260,
    DIV = 261,
    MOD = 262,
    ASSIGN = 263,
    EQ = 264,
    NEQ = 265,
    LE = 266,
    LEQ = 267,
    GE = 268,
    GEQ = 269,
    NOT = 270,
    AND = 271,
    OR = 272,
    NUM = 273,
    IDENT = 274,
    LBRAC = 275,
    RBRAC = 276,
    IF = 277,
    GOTO = 278,
    LABEL = 279,
    PARAM = 280,
    CALL = 281,
    RETURN = 282,
    COLON = 283,
    VAR = 284,
    FUNC = 285,
    END = 286
  };
#endif
/* Tokens.  */
#define ADD 258
#define SUB 259
#define MUL 260
#define DIV 261
#define MOD 262
#define ASSIGN 263
#define EQ 264
#define NEQ 265
#define LE 266
#define LEQ 267
#define GE 268
#define GEQ 269
#define NOT 270
#define AND 271
#define OR 272
#define NUM 273
#define IDENT 274
#define LBRAC 275
#define RBRAC 276
#define IF 277
#define GOTO 278
#define LABEL 279
#define PARAM 280
#define CALL 281
#define RETURN 282
#define COLON 283
#define VAR 284
#define FUNC 285
#define END 286

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HOME_XCW_XCW2_COMPILER_COMPILER_EEYORE2TIGGER_BUILD_XCW_PARSER_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  15
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   92

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  15
/* YYNRULES -- Number of rules.  */
#define YYNRULES  45
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  82

#define YYUNDEFTOK  2
#define YYMAXUTOK   286

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   175,   175,   176,   180,   184,   188,   195,   214,   244,
     262,   286,   293,   338,   349,   350,   354,   358,   362,   372,
     386,   399,   413,   434,   440,   446,   452,   459,   466,   472,
     480,   492,   493,   497,   498,   499,   500,   501,   505,   506,
     507,   508,   509,   510,   514,   531
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ADD", "SUB", "MUL", "DIV", "MOD",
  "ASSIGN", "EQ", "NEQ", "LE", "LEQ", "GE", "GEQ", "NOT", "AND", "OR",
  "NUM", "IDENT", "LBRAC", "RBRAC", "IF", "GOTO", "LABEL", "PARAM", "CALL",
  "RETURN", "COLON", "VAR", "FUNC", "END", "$accept", "Program",
  "ProgramUnit", "Declaration", "Initialization", "FunctionDef",
  "FunctionHeader", "FunctionEnd", "Statements", "Statement", "Expression",
  "BinOp", "OP", "LOGICOP", "RightValue", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286
};
# endif

#define YYPACT_NINF -22

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-22)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       8,     2,   -10,    -6,     1,   -22,   -22,   -22,   -22,    56,
      -1,     3,    -3,   -22,     6,   -22,   -22,     5,    10,    11,
       4,    10,     9,    10,   -22,    45,   -22,   -22,   -22,    13,
     -22,    15,     0,    10,   -22,   -22,    77,   -22,   -22,   -22,
     -22,   -22,    14,   -22,   -22,    32,   -22,   -22,   -22,   -22,
     -22,   -22,    21,    16,    10,    48,    22,   -22,   -22,   -22,
     -22,   -22,   -22,    10,   -22,    29,    10,   -22,   -22,    10,
     -22,   -22,    41,    27,   -22,    34,   -22,    10,    42,   -22,
     -22,   -22
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     2,     4,     5,     6,     0,
       0,     0,     0,     7,     0,     1,     3,     0,     0,     0,
       0,     0,     0,    28,    16,     0,    14,    17,     9,     0,
       8,     0,     0,     0,    45,    44,     0,    24,    25,    26,
      27,    29,     0,    11,    15,     0,    12,    34,    35,    36,
      37,    33,    44,     0,     0,    18,     0,    42,    43,    38,
      39,    40,    41,     0,    13,     0,     0,    30,    19,     0,
      31,    32,     0,     0,    10,     0,    20,     0,     0,    22,
      21,    23
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -22,   -22,    61,    -2,   -22,   -22,   -22,   -22,   -22,    52,
     -22,   -22,    18,    37,   -21
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,     7,     8,     9,    43,    25,    26,
      27,    69,    54,    63,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      39,    15,    41,    47,    48,    49,    50,    24,    12,    13,
      10,    55,    56,    32,    14,    51,    30,    28,    34,    52,
       1,    29,    11,    24,    31,    33,    53,     1,    34,    35,
       2,     3,    38,    68,    45,    37,    46,     2,     3,    40,
      65,    66,    73,    72,    64,    75,    67,    74,    76,    77,
      78,    47,    48,    49,    50,    79,    80,    57,    58,    59,
      60,    61,    62,    51,    17,    16,    81,    18,    19,    20,
      21,    22,    23,    70,     2,    17,    42,    44,    18,    19,
      20,    21,    22,    23,     0,     2,    57,    58,    59,    60,
      61,    62,    71
};

static const yytype_int8 yycheck[] =
{
      21,     0,    23,     3,     4,     5,     6,     9,    18,    19,
       8,    32,    33,     8,    20,    15,    19,    18,    18,    19,
      19,    18,    20,    25,    18,    20,    26,    19,    18,    19,
      29,    30,    28,    54,    21,    24,    21,    29,    30,    30,
       8,    20,    63,    21,    30,    66,    30,    18,    69,     8,
      23,     3,     4,     5,     6,    21,    77,     9,    10,    11,
      12,    13,    14,    15,    19,     4,    24,    22,    23,    24,
      25,    26,    27,    55,    29,    19,    31,    25,    22,    23,
      24,    25,    26,    27,    -1,    29,     9,    10,    11,    12,
      13,    14,    55
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    19,    29,    30,    33,    34,    35,    36,    37,    38,
       8,    20,    18,    19,    20,     0,    34,    19,    22,    23,
      24,    25,    26,    27,    35,    40,    41,    42,    18,    18,
      19,    18,     8,    20,    18,    19,    46,    24,    28,    46,
      30,    46,    31,    39,    41,    21,    21,     3,     4,     5,
       6,    15,    19,    26,    44,    46,    46,     9,    10,    11,
      12,    13,    14,    45,    30,     8,    20,    30,    46,    43,
      44,    45,    21,    46,    18,    46,    46,     8,    23,    21,
      46,    24
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    33,    34,    34,    34,    35,    35,    36,
      36,    37,    38,    39,    40,    40,    41,    41,    42,    42,
      42,    42,    42,    42,    42,    42,    42,    42,    42,    42,
      42,    43,    43,    44,    44,    44,    44,    44,    45,    45,
      45,    45,    45,    45,    46,    46
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     2,     3,     3,
       6,     3,     4,     2,     1,     2,     1,     1,     3,     4,
       5,     6,     6,     6,     2,     2,     2,     2,     1,     2,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 181 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        Out_Print("other");
    }
#line 1482 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 5:
#line 185 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        // Out_Print("init");
    }
#line 1490 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 6:
#line 189 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        Out_Print("other");
    }
#line 1498 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 7:
#line 196 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        if(Flag_def_out == 1){          //如果是全局变量，需要初始化为0
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr(yyvsp[0])),("v" + to_string(VAR_v_num)));
            VAR_v_num ++;
            tmp_ptr->IF_Global = 1;
            Scope.push_back(*tmp_ptr);
            other_out = IF_DEEP() + tmp_ptr->IR_name + " = 0";
            Func_Other.push_back(other_out);
        }
        else{
            // out << "in else " + *(ToStr($2)) << endl;
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr(yyvsp[0])),"");
            tmp_ptr->IR_name = to_string(Stack_Func_size);
            tmp_ptr->IF_Global = 0;
            Scope.push_back(*tmp_ptr);
        }
        Stack_Func_size ++;    //函数需要的栈空间 + 1
    }
#line 1521 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 8:
#line 215 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        if(Flag_def_out == 1){
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr(yyvsp[0])),("v" + to_string(VAR_v_num)));
            VAR_v_num ++;
            tmp_ptr->IF_Global = 1;
            tmp_ptr->Array_size = *ToInt(yyvsp[-1]);
            tmp_ptr->IDENT_if_array = 1;
            Scope.push_back(*tmp_ptr);

            other_out = IF_DEEP() + tmp_ptr->IR_name + " = malloc " + to_string(tmp_ptr->Array_size);
            Func_Other.push_back(other_out);

            Stack_Func_size ++;    //函数需要的栈空间 + 1
        }
        // out << "IR_name = "<<tmp_ptr->IR_name<<endl;
        else{
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr(yyvsp[0])),"");
            tmp_ptr->Array_size = *ToInt(yyvsp[-1]);
            tmp_ptr->IDENT_if_array = 1;
            tmp_ptr->IF_Global = 0;
            tmp_ptr->IR_name = to_string(Stack_Func_size);
            Stack_Func_size += tmp_ptr->Array_size;    //函数需要的栈空间 + 1
            Scope.push_back(*tmp_ptr);
            // out << "in else " + *(ToStr($3)) << endl;
        }
    }
#line 1552 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 9:
#line 245 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        DEEP = 1;
        if(Flag_init_in_func==0 && Flag_IF_nfunc == 0){
            init_out = "f_init_nfunc [0] [0]";       //单纯用来输出定义
            Func_Init.push_back(init_out);
            Flag_IF_nfunc = 1;
        }
        IDENT_scope* tmp_ptr = find_define(*(ToStr(yyvsp[-2])));
        init_out = IF_DEEP() + "loadaddr " + tmp_ptr->IR_name + " t0";
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t1 = " + to_string(*ToInt(yyvsp[0]));
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t0[0] = t1";
        Func_Init.push_back(init_out);

        DEEP = 0;
    }
#line 1574 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 10:
#line 263 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        DEEP = 1;
        if(Flag_init_in_func==0 && Flag_IF_nfunc == 0){
            init_out = "f_init_nfunc [0] [0]";       //单纯用来输出定义
            Func_Init.push_back(init_out);
            Flag_IF_nfunc = 1;
        }

        IDENT_scope* tmp_ptr = find_define(*(ToStr(yyvsp[-5])));

        init_out = IF_DEEP() + "loadaddr " + tmp_ptr->IR_name + " p0";
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t1 = " + to_string(*(ToInt(yyvsp[0])));
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t0[" + to_string(*ToInt(yyvsp[-3])) + "] = t1";
        Func_Init.push_back(init_out);
        

        DEEP = 0;
    }
#line 1599 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 11:
#line 287 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        Func_Other[0] += ("[" + to_string(Stack_Func_size) + "]");
    }
#line 1607 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 12:
#line 294 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        Flag_def_out = 0;    //表示已经在函数内部
        Stack_Func_size = 0;

        if((*(ToStr(yyvsp[-3]))) == "f_main"){
            if(!Func_Init.empty()){
                init_out = "\treturn";       //单纯用来输出定义
                Func_Init.push_back(init_out);
                init_out = "end f_init_nfunc";       
                Func_Init.push_back(init_out);
                Out_Print("init");      //如果遇到main函数，就先把之前的初始化语句在函数init_nfunc中输出来
            }
            
        }

        DEEP ++;

        IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr(yyvsp[-3])),"");
        tmp_ptr->Param_num = *ToInt(yyvsp[-1]);
        Scope.push_back(*tmp_ptr);
        Loc_Func_def = Func_Other.size();     //记录当前函数最后的位置，用来插入定义语句
        Stack_Func_nparam = *ToInt(yyvsp[-1]);    // 初始化为参数的大小
        other_out = (*(ToStr(yyvsp[-3]))) + " [" + to_string(*ToInt(yyvsp[-1])) + "] ";
        Func_Other.push_back(other_out);

        while(Stack_Func_size < Stack_Func_nparam){
            other_out = IF_DEEP() + "store a" + to_string(Stack_Func_size) + " " + to_string(Stack_Func_size);
            Func_Other.push_back(other_out);
            Stack_Func_size ++;
        }

        if((*(ToStr(yyvsp[-3]))) == "f_main"){
            if(Flag_IF_nfunc == 1){
                other_out = IF_DEEP() + "call f_init_nfunc";
                Func_Other.push_back(other_out);
            }
            
        }

        
    }
#line 1653 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 13:
#line 339 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        other_out = "end " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);   //输出end
        DEEP --;

        Flag_def_out = 1;
    }
#line 1665 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 16:
#line 355 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        
    }
#line 1673 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 18:
#line 363 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr(yyvsp[-2])));
        
        IDENT_Assign(tmp_ptr1, (*(ToStr(yyvsp[0]))));
        // other_out = IF_DEEP() + "store " + (*(ToStr($3))) + " " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
#line 1687 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 19:
#line 373 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr(yyvsp[-3])));

        other_out = IF_DEEP() + "s0 = " + (*ToStr(yyvsp[-1])) + " " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);

        IDENT_Assign(tmp_ptr1, "s0");
        
        // other_out = IF_DEEP() + "store s0 " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
#line 1705 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 20:
#line 387 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr(yyvsp[-4])));

        other_out = IF_DEEP() + "s0 = " + (*ToStr(yyvsp[-2])) + " " + (*ToStr(yyvsp[-1])) + " " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);

        IDENT_Assign(tmp_ptr1, "s0");
        // other_out = IF_DEEP() + "store s0 " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
#line 1722 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 21:
#line 400 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr(yyvsp[-5])));
        other_out = IF_DEEP() + "loadaddr " + tmp_ptr1->IR_name + " s0";
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + "s0 = s0 + " + (*(ToStr(yyvsp[-3])));
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + "s0[0] = " + (*(ToStr(yyvsp[0])));
        Func_Other.push_back(other_out);

        s_num = 1;
    }
#line 1740 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 22:
#line 414 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr(yyvsp[-5])));
        IDENT_scope* tmp_ptr2 = find_define(*(ToStr(yyvsp[-3])));

        string* tmp_name = new string("s" + to_string(s_num));
        other_out = IF_DEEP() + "load " + tmp_ptr2->IR_name + " " + (*tmp_name);
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + (*tmp_name) + " = " + (*tmp_name) + " + "+ (*(ToStr(yyvsp[-1])));
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + "s0 = " + (*tmp_name) + "[0]";
        Func_Other.push_back(other_out);

        IDENT_Assign(tmp_ptr1, "s0");
        // other_out = IF_DEEP() + "store s0 " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
#line 1765 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 23:
#line 435 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        other_out = IF_DEEP() + "if " + (*ToStr(yyvsp[-4])) + " " + (*ToStr(yyvsp[-3])) + " " + (*ToStr(yyvsp[-2])) + " goto " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);
        s_num = 1;
    }
#line 1775 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 24:
#line 441 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        other_out = IF_DEEP() + "goto " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);
        s_num = 1;
    }
#line 1785 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 25:
#line 447 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        other_out = IF_DEEP() + (*ToStr(yyvsp[0])) + ":";
        Func_Other.push_back(other_out);
        s_num = 1;
    }
#line 1795 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 26:
#line 453 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        other_out = IF_DEEP() + "a" + to_string(VAR_a_num) + " = " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);
        VAR_a_num ++;
        s_num = 1;
    }
#line 1806 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 27:
#line 460 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        VAR_a_num = 0;
        other_out = IF_DEEP() + "call " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);
        s_num = 1;
    }
#line 1817 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 28:
#line 467 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        other_out = IF_DEEP() + "return";
        Func_Other.push_back(other_out);
        s_num = 1;
    }
#line 1827 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 29:
#line 473 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        other_out = IF_DEEP() + "a0 = " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);
        other_out = IF_DEEP() + "return";
        Func_Other.push_back(other_out);
        s_num = 1;
    }
#line 1839 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 30:
#line 481 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        VAR_a_num = 0;
        other_out = IF_DEEP() + "call " + (*ToStr(yyvsp[0]));
        Func_Other.push_back(other_out);
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr(yyvsp[-3])));
        IDENT_Assign(tmp_ptr1, "a0");
        s_num = 1;
    }
#line 1852 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 44:
#line 515 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        // Ptr_num* tmp_ptr = new Ptr_num(*(ToStr($1)));
        // $$ = (void*)(tmp_ptr);
        IDENT_scope* tmp_ptr2 = find_define(*(ToStr(yyvsp[0])));
        if(tmp_ptr2->IDENT_if_array){
            other_out = IF_DEEP() + "loadaddr " + tmp_ptr2->IR_name + " s" + to_string(s_num);
            Func_Other.push_back(other_out);
        }
        else{
            other_out = IF_DEEP() + "load " + tmp_ptr2->IR_name + " s" + to_string(s_num);
            Func_Other.push_back(other_out);
        }
        string* str = new string("s" + to_string(s_num));
        yyval = (void*)(str);
        s_num ++;
    }
#line 1873 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;

  case 45:
#line 532 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1652  */
    {
        // Ptr_num* tmp_ptr = new Ptr_num(*(ToInt($1)));
        // $$ = (void*)(tmp_ptr);
        other_out = IF_DEEP() + "s" + to_string(s_num) + " = " + to_string(*ToInt(yyvsp[0]));
        Func_Other.push_back(other_out);
        string* str = new string("s" + to_string(s_num));
        yyval = (void*)(str);
        s_num ++;
    }
#line 1887 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
    break;


#line 1891 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/build/xcw_parser.tab.c" /* yacc.c:1652  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 548 "/home/xcw/xcw2_Compiler/Compiler_Eeyore2Tigger/source/tigger_parser.y" /* yacc.c:1918  */


void yyerror(const char *s) {
    cout << "Line " << yylineno << "," << charNum << ": " << s << endl;
    exit(1);
}

void yyerror(const string &s) {
    yyerror(s.c_str());
}

int main(int argc, char **argv) {
    if (argc >= 4)
        if ((yyin = fopen(argv[3], "r")) == NULL)
            yyerror("Cannot open input file.");
    
    if (argc >= 6)
        if (freopen(argv[5], "w", stdout) == NULL)
            yyerror("Cannot open output file.");

    yyparse();

    return 0;
}
