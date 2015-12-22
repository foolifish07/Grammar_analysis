# Grammar analysis 

标签（空格分隔）： Gramma

---

14年的软件课程设计（二）
具体要求在Task of  Compiling Project.doc里面、
其实就是做一个语法词法分析。。扔这儿了吧、、

# 详细设计说明书

## 文件说明
* refer 参考的BNF文法
* compile_fooc.cpp 词法+文法源代码
* grammer_C.txt 初版fooC语法
* syntax_grammar.txt  LL1 fooC语法
* lex_grammar.txt  fooC词法
* lex_table.data  词法输出token表
* source_code.fooc 待分析、识别的fooC代码
* lex_grammar.cpp 原本欲使用的3型产生器

## YY的Grammar
要分析词法语法，首先要、、有一个文法、、
YY了一个、约定如下
首先，我参照了网上C Yacc grammar等BNF规范，用“：”代替了书本上的“-》”
符号，这样方便了读入也，方便了文法的书写。
接下来一个关键的问题是，如何在文法中确定单个元素？在这点上，我决定用空格分离。
接着我们的问题是：文法中有“终结符”和“非终结符”，在这点上我又如何才能确立区分它们呢？想树上那样丑陋的用大小写字母确立显然是不现实的。这样就完全浪费了我的大写字母。考虑到我们的文法，是一群2型文法，所以在production左边的肯定都是non-terminal.。那么对于production右边的元素呢？很简单！如果production右边元素已经被判别为non-terminal，也就是在production左边出现过了，那它就是non-terminal，反之，其就是一个terminal啦！
当然我们的文法，还应当支持 |（或符号） 符号，对于一个长长的文法，我用“\”标识它的结束。而推导出空时，我用$来表示空。所以最后的文法类似如下情形：

> identifier 
>	: digit identifier
>	| latin identifier
>	| _ identifier
>	| $
>	\

还有就是“##” 开始的部分作为foolifishc文法的注释开头，同样将以##结尾如下：

> *##*
> *#*	foolifishC lex grammar 
> *#*	Using $ for empty
> *##*

作为注释，输入程序自然将不会鸟它。
为了简单起见，我将废除”|”,”$”,”##”，“\”。也就是说foolifishC中将不会有这些东西出现。
YY文法的过程中参考了很多语言的文法，我全部扔到refer里了、、

## Lex analysis
文法确立完了，接下来我们就要开始考虑分析器构造的问题啦。
对语法分析来说，如果还一个一个字母的分析会显得很麻烦，语法分析多需要的元素将应当是一些一整块的字符串，例如int，fasdd_df，“『』”等，这样在转化中将变得容易许多。
所以首先我们将进行Lex analysis，也就是词法分析。，它将至少识别例如int、1234567这样的一个整串。

## Syntax analysis
整个分析串中最重要的部分，我想就莫过于此了。因为词法分析不过是为了这语法分析而存在的。本次，我们用LL1方法实现。这也就要求我们的grammar是LL1的。为了消除左递归以及，合并左串，我们的grammar中奖必不可少的出现很多$，也就是推出空的production。
所谓，LL1，就是说当你如入下一个LEX 给你的element的时候你将自动得到应当接下来用那个production，并且还没有冲突。这样的话我们就需要知道每个产生式的首字母是啥。也就是SELECT(production)是什么。接下来一切就都好操作了。
我的代码的流程如下
1、 先求出每个non-terminal的FIRST集，也就是他们的开头元素的情况
2、 求出每个non-terminal的FOLLOW集，也也就是他们的接下来一个元素出现的情况，这一步必不可少，因为有大量的空推导。
3、 最后根据non-terminal们的FIRST集,和FOLLOW集搞出每个产生式的SELECT集合。
4、 对同一个non-terminal的所有production的SELECT集，我们进行判重，如果有重复，这就将不是一个LL1文法。
5、 如果都没有重复，我们就可以对每个non-terminal开始建边啦！

> 总的流程图
1、读入文法
2、找到所有的non-ternimal
3、识别剩余的ternimal
4、得到FIRST
5、得到FOLLOW
6、得到SELECT
7、对每个non-terminal判重
如果两个产生式推到有重，报错，不是LL1
8、对每个non-terminal建边
9、导入待分析字符串，判断。


## 有关效率
在FIRST、FOLLOW、SELECT的统计下一个、开头元素的时候，考虑到效率问题，我采用了bitset<maxn>，即开了一个长度为maxn的二进制空间。每一位上对应一个terminal的情况，0表示没有，1表示有。外部用map做terminal--->数字ID的转化。

## 回头有关LEX_GRAMMAR
Task里面有关说，Lex_grammer必须是3型文法，我表示非常困惑。因为要做一个稍微实在些的词法文法，用3型文法书写，实在太花费时间、而且是比不必要的时间。我曾想通过代码生成3型文法，但回头转念一想，何必呢？本来手写2型文法能够轻松构造出的词法，我还非要用代码将它拖开为3型文法，这不是没事找事吗？而关于DFA以及NFA的问题，无论词法分析还是语法分析，最后终将用DFA进行分析。而LL1本身就是一个DFA，而LR1则能够转化一些NFA为DFA。而3型文法不一定是一个DFA，却一定是LR1的。
故本次，我不再为，lex grammar在特别设立一个输入格式。而是直接套用语法的输入。继续采用了兼容3型文法的2型文法输入。

# 代码解释

```cpp
map<string,int> vt, vn;
struct NFA{
		string name;
		vector< pair<int,int> > edge;
		bitset<maxn> first , follow;
		map<int,int> to;

		vector< vector<string> >production;
		vector< bitset<maxn> > select;
	};
一个NFA状态。显然一个非终结符就是一个状态。
```

>Name 表示非终结符的名称
FIRST、FOLLOW、非终结符的FIRST和FOLLOW集
SELECT、PRODUCTION 以此non-terminal为左部的产生式以及SELECT 集合。

```cpp

//读入一个foolifish文法，判断是否为foolifish文法
//输入完成后调用get_terminal
bool read_grammar(string filename)

//得到终结符
void get_terminal() 

//判断一个元素串是否为终结符
bool isTerminal(string s) 

//得到非终结符的FIRST集合
void get_firstset() 

//布尔类型。以start为开头。开头会先调用get_firstset
bool get_followset()

//得到每个产生式的select集合 开头调用get_followset
bool get_selectset() 

/*
得到to映射表。即当输入下一个非终结符时，使用那个产生式进行跳转。
如果同一个非终结符的两个产生式的SELECT集合有冲突，将打印不是LL1文法的编译信息，并return fales
开头调用selectset */
bool build_edge()

//初始化。调用read_grammar以及build_edge
bool init(string s)

/* 编译分析带分析串。结果打印到outfile里面 openlog为参数，可打印分析日志。 */
bool compile_source_code(string inputfile , string outfile ,int openlog)


/*词法分析模块，读入文法。分析带分析串。*/
bool lex_analysis(string lex_grammar, string lex_table,string source_code, int openlog)

// 从inputfile里面读入lex的分析结果。Token表
bool compile_lex_table(string inputfile , int openlog)

// 语法分析模块。读入语法文法。分析Token表。
bool syntax_analysis(string syntax_grammar, string lex_table, int openlog)

```

## 调用说明

一份源代码compile_fooc.cpp
编译器mingw4.8
程序需要四个文件

```cpp
//词法文法文件
char lex_grammar[maxn] = "lex_grammar.txt";
//词法分析完成得到的token table
char lex_table[maxn] = "lex_table.data";
//语法文法文件
char syntax_grammar[maxn] = "syntax_grammar.txt";
//待分析字符串
char source_code[maxn] = "source_code.fooc";
```

另有个参数
```cpp
int openlog = 0;
```
是否需要打印程序运行日志 0:否 1:是
调用如下：
```cpp
compile.lex_analysis(lex_grammar , lex_table ,source_code,openlog)
```

语法分析

```cpp
compile.syntax_analysis( syntax_grammar,lex_table,openlog )
```
两者都是 成功TRUE，失败FALSE
