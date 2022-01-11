# 2、第二部分 Eeyore -> Tigger

这部分为了方便，采用了暴力将所有变量放入内存的做法

## 1）主要数据结构

### 符号相关

符号表和`IDENT_scope`和第一阶段类似，去掉了在第二部分中没用的变量

```c
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
```

### 变量相关

注意tigger只允许在函数中赋值，所以需要标记是否在函数中

```c
int Flag_init_in_func;            // 表示变量初始化是否在函数中
int Flag_def_out = 1;            // 表示变量是否为全局变量
```

用全局变量记录函数栈空间相关的变量

```c
int Stack_Func_size;    //函数需要栈空间的大小
int Stack_Func_nparam;    //已经被占用的栈空间的大小

int Func_stack_num;    //包括所有的参数和定义的变量，函数定义结束后从scope中弹出
```

用`VAR_a_num`记录函数参数寄存器的个数，设置`s_num = 1`始终保留s0用于最后的返回

```c
int s_num = 1;    // 始终保留s0用于最后的返回
int VAR_a_num = 0;    // 函数参数寄存器
```

其他定义和变量基本完全相同

### 工具函数

用函数`IDENT_Assign`区别全局变量和局部变量的赋值，如果是非数组的局部变量，用`store`来进行赋值，如果是其他，则通过`loadaddra`从内存中读出地址进行赋值

```c
void IDENT_Assign(IDENT_scope* tmp1, string str2 ){    //区别全局变量和局部变量
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
```

## 2）全局变量初始化

因为在tigger中，对变量的赋值需要在函数中进行，所以考虑对于存在全局变量赋值的输入，构造函数`init_nfunc`专门用于这些全局变量的赋值，将其放在`Func_Init`中。`Initialization->IDENT ASSIGN NUM`为例

```c
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
```

然后在遇到`main`函数的时候，输出`Func_Init`的所有内容

```c
if((*(ToStr($1))) == "f_main"){
  if(!Func_Init.empty()){
    init_out = "\treturn";       //单纯用来输出定义
    Func_Init.push_back(init_out);
    init_out = "end f_init_nfunc";       
    Func_Init.push_back(init_out);
    Out_Print("init");      //如果遇到main函数，就先把之前的初始化语句在函数init_nfunc中输出来
  }

}
```

## 3）函数中栈的处理

首先处理参数问题，`Stack_Func_size`初始为0，`Stack_Func_nparam`表示参数个数

先将参数放到符号表中，并存入其在函数的栈中的位置。

```c
while(Stack_Func_size < Stack_Func_nparam){
  other_out = IF_DEEP() + "store a" + to_string(Stack_Func_size) + " " + to_string(Stack_Func_size);
  Func_Other.push_back(other_out);

  IDENT_scope* tmp_param = new IDENT_scope("p"+to_string(Stack_Func_size) ,"");
  tmp_param->IR_name = to_string(Stack_Func_size);
  tmp_param->IF_Global = 0;
  Scope.push_back(*tmp_param);
  Stack_Func_size ++;
}
```

## 4）地址和值的区别

赋值语句中，在从内存中读取内容的时候，需要区别是读出地址还是这个地址存的值。如果左边的数是数组类型，用`loadaddr`，否则用`load`。以`Expression->IDENT LBRAC RightValue RBRAC ASSIGN RightValue`为例

```c
IDENT LBRAC RightValue RBRAC ASSIGN RightValue
{
  IDENT_scope* tmp_ptr1 = find_define(*(ToStr($1)));
  if(tmp_ptr1->IDENT_if_array){
    other_out = IF_DEEP() + "loadaddr " + tmp_ptr1->IR_name + " s0";
    Func_Other.push_back(other_out);
  }
  else{
    other_out = IF_DEEP() + "load " + tmp_ptr1->IR_name + " s0";
    Func_Other.push_back(other_out);
  }

  other_out = IF_DEEP() + "s0 = s0 + " + (*(ToStr($3)));
  Func_Other.push_back(other_out);

  other_out = IF_DEEP() + "s0[0] = " + (*(ToStr($6)));
  Func_Other.push_back(other_out);

  s_num = 1;
}
```

