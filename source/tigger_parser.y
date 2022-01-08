%{
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


struct IDENT_scope{
    string IDENT_name;
    string IDENT_num;          // 变量的值可变，因此用string存储
    string IR_name;          // 在tigger中的变量名

    int Array_size;
    bool IDENT_if_array;       //是否为数组变量

    IDENT_scope(string name,string ir_name){       //常量的构造函数
        IDENT_name = name;
        IR_name = ir_name;
        IDENT_if_array = 0;    // 默认不是数组
    }

};

vector<IDENT_scope> Scope;     //符号表

int VAR_v_num = 0;

//-----------------函数语句打印相关变量------------------------------
vector<string> Func_VarDecl;
vector<string> Func_Other;
string def_out;       // 这个string 用来记录用于def的语句
string other_out;     // 这个string 用来记录其他的语句
void Out_Print(){
    for(int i = 0;i < Func_VarDecl.size();i++){
        out << Func_VarDecl[i] << endl;
    }
    for(int i = 0;i < Func_Other.size();i++){
        out << Func_Other[i] << endl;
    }
    //out << func_def_out << endl;
    Func_VarDecl.clear();
    Func_Other.clear();
}

%}

%token ADD SUB MUL DIV MOD
%token ASSIGN EQ NEQ LE LEQ GE GEQ NOT AND OR
%token NUM IDENT
%token LBRAC RBRAC
%token IF GOTO LABEL PARAM CALL RETURN COLON VAR FUNC END


%%

Program:
    ProgramUnit
    | Program ProgramUnit
;

ProgramUnit:
    Declaration
    {
        Out_Print();
    }
;

Declaration:
    VAR IDENT
    {
        IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($2)),("v" + to_string(VAR_v_num)));
        VAR_v_num ++;
        Scope.push_back(*tmp_ptr);
        def_out = tmp_ptr->IR_name + " = 0";
        Func_VarDecl.push_back(def_out);

    }
    | VAR NUM IDENT
    {
        IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($3)),("v" + to_string(VAR_v_num)));
        VAR_v_num ++;
        tmp_ptr->Array_size = *ToInt($2);
        tmp_ptr->IDENT_if_array = 1;

        def_out = tmp_ptr->IR_name + " = malloc " + to_string(tmp_ptr->Array_size);
        Func_VarDecl.push_back(def_out);
    }
;










%%

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