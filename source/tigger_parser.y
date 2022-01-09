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
            tmp_ptr->IF_Global = 1;
            Scope.push_back(*tmp_ptr);
            other_out = IF_DEEP() + tmp_ptr->IR_name + " = 0";
            Func_Other.push_back(other_out);
        }
        else{
            // out << "in else " + *(ToStr($2)) << endl;
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($2)),"");
            tmp_ptr->IR_name = to_string(Stack_Func_size);
            tmp_ptr->IF_Global = 0;
            Scope.push_back(*tmp_ptr);
        }
        Stack_Func_size ++;    //函数需要的栈空间 + 1
    }
    | VAR NUM IDENT
    {
        if(Flag_def_out == 1){
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($3)),("v" + to_string(VAR_v_num)));
            VAR_v_num ++;
            tmp_ptr->IF_Global = 1;
            tmp_ptr->Array_size = *ToInt($2);
            tmp_ptr->IDENT_if_array = 1;
            Scope.push_back(*tmp_ptr);

            other_out = IF_DEEP() + tmp_ptr->IR_name + " = malloc " + to_string(tmp_ptr->Array_size);
            Func_Other.push_back(other_out);

            Stack_Func_size ++;    //函数需要的栈空间 + 1
        }
        // out << "IR_name = "<<tmp_ptr->IR_name<<endl;
        else{
            IDENT_scope* tmp_ptr = new IDENT_scope(*(ToStr($3)),"");
            tmp_ptr->Array_size = *ToInt($2);
            tmp_ptr->IDENT_if_array = 1;
            tmp_ptr->IF_Global = 0;
            tmp_ptr->IR_name = to_string(Stack_Func_size);
            Stack_Func_size += tmp_ptr->Array_size;    //函数需要的栈空间 + 1
            Scope.push_back(*tmp_ptr);
            // out << "in else " + *(ToStr($3)) << endl;
        }
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
        Stack_Func_size = 0;

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
        Stack_Func_nparam = *ToInt($3);    // 初始化为参数的大小
        other_out = (*(ToStr($1))) + " [" + to_string(*ToInt($3)) + "] ";
        Func_Other.push_back(other_out);

        while(Stack_Func_size < Stack_Func_nparam){
            other_out = IF_DEEP() + "store a" + to_string(Stack_Func_size) + " " + to_string(Stack_Func_size);
            Func_Other.push_back(other_out);
            Stack_Func_size ++;
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
        
    }
    | Expression
;

Expression:
    IDENT ASSIGN RightValue
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr($1)));
        
        IDENT_Assign(tmp_ptr1, (*(ToStr($3))));
        // other_out = IF_DEEP() + "store " + (*(ToStr($3))) + " " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
    | IDENT ASSIGN OP RightValue
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr($1)));

        other_out = IF_DEEP() + "s0 = " + (*ToStr($3)) + " " + (*ToStr($4));
        Func_Other.push_back(other_out);

        IDENT_Assign(tmp_ptr1, "s0");
        
        // other_out = IF_DEEP() + "store s0 " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
    | IDENT ASSIGN RightValue BinOp RightValue
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr($1)));

        other_out = IF_DEEP() + "s0 = " + (*ToStr($3)) + " " + (*ToStr($4)) + " " + (*ToStr($5));
        Func_Other.push_back(other_out);

        IDENT_Assign(tmp_ptr1, "s0");
        // other_out = IF_DEEP() + "store s0 " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
    | IDENT LBRAC RightValue RBRAC ASSIGN RightValue
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr($1)));
        other_out = IF_DEEP() + "loadaddr " + tmp_ptr1->IR_name + " s0";
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + "s0 = s0 + " + (*(ToStr($3)));
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + "s0[0] = " + (*(ToStr($6)));
        Func_Other.push_back(other_out);

        s_num = 1;
    }
    | IDENT ASSIGN IDENT LBRAC RightValue RBRAC 
    {
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr($1)));
        IDENT_scope* tmp_ptr2 = find_define(*(ToStr($3)));

        string* tmp_name = new string("s" + to_string(s_num));
        other_out = IF_DEEP() + "load " + tmp_ptr2->IR_name + " " + (*tmp_name);
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + (*tmp_name) + " = " + (*tmp_name) + " + "+ (*(ToStr($5)));
        Func_Other.push_back(other_out);

        other_out = IF_DEEP() + "s0 = " + (*tmp_name) + "[0]";
        Func_Other.push_back(other_out);

        IDENT_Assign(tmp_ptr1, "s0");
        // other_out = IF_DEEP() + "store s0 " + tmp_ptr1->IR_name;
        // Func_Other.push_back(other_out);

        s_num = 1;
    }
    | IF RightValue LOGICOP RightValue GOTO LABEL
    {
        other_out = IF_DEEP() + "if " + (*ToStr($2)) + " " + (*ToStr($3)) + " " + (*ToStr($4)) + " goto " + (*ToStr($6));
        Func_Other.push_back(other_out);
        s_num = 1;
    }
    | GOTO LABEL
    {
        other_out = IF_DEEP() + "goto " + (*ToStr($2));
        Func_Other.push_back(other_out);
        s_num = 1;
    }
    | LABEL COLON
    {
        other_out = IF_DEEP() + (*ToStr($2)) + ":";
        Func_Other.push_back(other_out);
        s_num = 1;
    }
    | PARAM RightValue
    {
        other_out = IF_DEEP() + "a" + to_string(VAR_a_num) + " = " + (*ToStr($2));
        Func_Other.push_back(other_out);
        VAR_a_num ++;
        s_num = 1;
    }
    | CALL FUNC
    {
        VAR_a_num = 0;
        other_out = IF_DEEP() + "call " + (*ToStr($2));
        Func_Other.push_back(other_out);
        s_num = 1;
    }
    | RETURN
    {
        other_out = IF_DEEP() + "return";
        Func_Other.push_back(other_out);
        s_num = 1;
    }
    | RETURN RightValue
    {
        other_out = IF_DEEP() + "a0 = " + (*ToStr($2));
        Func_Other.push_back(other_out);
        other_out = IF_DEEP() + "return";
        Func_Other.push_back(other_out);
        s_num = 1;
    }
    | IDENT ASSIGN CALL FUNC
    {
        VAR_a_num = 0;
        other_out = IF_DEEP() + "call " + (*ToStr($4));
        Func_Other.push_back(other_out);
        IDENT_scope* tmp_ptr1 = find_define(*(ToStr($1)));
        IDENT_Assign(tmp_ptr1, "a0");
        s_num = 1;
    } 
;

BinOp:
    OP
    | LOGICOP
;

OP:
    NOT
    | ADD
    | SUB
    | MUL
    | DIV
;

LOGICOP:
    LE
    | LEQ
    | GE
    | GEQ
    | EQ
    | NEQ
;

RightValue:
    IDENT
    {
        // Ptr_num* tmp_ptr = new Ptr_num(*(ToStr($1)));
        // $$ = (void*)(tmp_ptr);
        IDENT_scope* tmp_ptr2 = find_define(*(ToStr($1)));
        if(tmp_ptr2->IDENT_if_array){
            other_out = IF_DEEP() + "loadaddr " + tmp_ptr2->IR_name + " s" + to_string(s_num);
            Func_Other.push_back(other_out);
        }
        else{
            other_out = IF_DEEP() + "load " + tmp_ptr2->IR_name + " s" + to_string(s_num);
            Func_Other.push_back(other_out);
        }
        string* str = new string("s" + to_string(s_num));
        $$ = (void*)(str);
        s_num ++;
    }
    | NUM
    {
        // Ptr_num* tmp_ptr = new Ptr_num(*(ToInt($1)));
        // $$ = (void*)(tmp_ptr);
        other_out = IF_DEEP() + "s" + to_string(s_num) + " = " + to_string(*ToInt($1));
        Func_Other.push_back(other_out);
        string* str = new string("s" + to_string(s_num));
        $$ = (void*)(str);
        s_num ++;
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