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
    int Param_num;
    int Stack_loc;       //在函数中的位置

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

//---------------------函数回填相关----------------
int Loc_Func_def;    //函数被定义的位置，用于最后的回填  
int Stack_Func_size;    //函数需要栈空间的大小
int Stack_Func_inuse;    //已经被占用的栈空间的大小



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
        Out_Print("other");
    }
    | Initialization
    {
        // Out_Print("init");
    }
    | FunctionDef
    {
        Out_Print("other");
    }
;

Declaration:
    VAR IDENT
    {
        if(Flag_def_out == 1){          //如果是全局变量，需要初始化为0
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($2)),("v" + to_string(VAR_v_num)));
            VAR_v_num ++;
            Scope.push_back(*tmp_ptr);
            other_out = IF_DEEP() + tmp_ptr->IR_name + " = 0";
            Func_Other.push_back(other_out);
        }
        else{
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($2)),"");
            tmp_ptr->Stack_loc = Stack_Func_size;
        }

    }
    | VAR NUM IDENT
    {
        IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($3)),("v" + to_string(VAR_v_num)));
        VAR_v_num ++;
        tmp_ptr->Array_size = *ToInt($2);
        tmp_ptr->IDENT_if_array = 1;
        Scope.push_back(*tmp_ptr);
        // out << "IR_name = "<<tmp_ptr->IR_name<<endl;

        other_out = IF_DEEP() + tmp_ptr->IR_name + " = malloc " + to_string(tmp_ptr->Array_size);
        Func_Other.push_back(other_out);
    }
;

Initialization:
    IDENT ASSIGN NUM
    {
        DEEP = 1;
        if(Flag_init_in_func==0 && Flag_IF_nfunc == 0){
            init_out = "f_init_nfunc [0] [0]";       //单纯用来输出定义
            Func_Init.push_back(init_out);
            Flag_IF_nfunc = 1;
        }
        IDENT_scope* tmp_ptr = find_define(*(ToStr($1)));
        init_out = IF_DEEP() + "loadaddr " + tmp_ptr->IR_name + " t0";
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t1 = " + to_string(*ToInt($3));
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t0[0] = t1";
        Func_Init.push_back(init_out);

        DEEP = 0;
    }
    | IDENT LBRAC NUM RBRAC ASSIGN NUM
    {
        DEEP = 1;
        if(Flag_init_in_func==0 && Flag_IF_nfunc == 0){
            init_out = "f_init_nfunc [0] [0]";       //单纯用来输出定义
            Func_Init.push_back(init_out);
            Flag_IF_nfunc = 1;
        }

        IDENT_scope* tmp_ptr = find_define(*(ToStr($1)));

        init_out = IF_DEEP() + "loadaddr " + tmp_ptr->IR_name + " p0";
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t1 = " + to_string(*(ToInt($6)));
        Func_Init.push_back(init_out);
        init_out = IF_DEEP() + "t0[" + to_string(*ToInt($3)) + "] = t1";
        Func_Init.push_back(init_out);
        

        DEEP = 0;
    }
;

FunctionDef:
    FunctionHeader Statements FunctionEnd
    {
        Func_Other[0] += ("[" + to_string(Stack_Func_size) + "]");
    }
; 

FunctionHeader:
    FUNC LBRAC NUM RBRAC
    {
        Flag_def_out = 0;    //表示已经在函数内部
        Stack_Func_inuse = 0;   //已占用的栈空间初始化为0

        if((*(ToStr($1))) == "f_main"){
            init_out = "return";       //单纯用来输出定义
            Func_Init.push_back(init_out);
            Out_Print("init");      //如果遇到main函数，就先把之前的初始化语句在函数init_nfunc中输出来
        }

        DEEP ++;

        IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($1)),"");
        tmp_ptr->Param_num = *ToInt($3);
        Scope.push_back(*tmp_ptr);
        Loc_Func_def = Func_Other.size();     //记录当前函数最后的位置，用来插入定义语句
        Stack_Func_size = *ToInt($3);    // 初始化为参数的大小
        other_out = (*(ToStr($1))) + " [" + to_string(*ToInt($3)) + "] ";
        Func_Other.push_back(other_out);

        while(Stack_Func_inuse < Stack_Func_size){
            other_out = IF_DEEP() + "store a" + to_string(Stack_Func_inuse) + " " + to_string(Stack_Func_inuse);
            Func_Other.push_back(other_out);
            Stack_Func_inuse ++;
        }

        if((*(ToStr($1))) == "f_main"){
            if(Flag_IF_nfunc == 1){
                other_out = IF_DEEP() + "call f_init_nfunc";
                Func_Other.push_back(other_out);
            }
            
        }

        
    }
;

FunctionEnd:
    END FUNC
    {
        other_out = "end " + (*ToStr($2));
        Func_Other.push_back(other_out);   //输出end
        DEEP --;

        Flag_def_out = 1;
    }
;

Statements:
    Statement
    | Statements Statement
;

Statement:
    Declaration
    {
        Stack_Func_size ++;    //函数需要的栈空间 + 1
    }
    | Expression
;

Expression:
    IDENT ASSIGN NUM
    {     //对应 T0 = 1 的情况
        // other_out = "tttttttttttttest--------------";
        // Func_Other.push_back(other_out);
        IDENT_scope* tmp_ptr = find_define(*(ToStr($1)));
        other_out = IF_DEEP() + "s1 = " + to_string(*ToInt($3));
        Func_Other.push_back(other_out);
        other_out = IF_DEEP() + "store s1 " + to_string(tmp_ptr->Stack_loc);
        Func_Other.push_back(other_out);
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