#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <string.h>
//常量声明
//*12311405 此时的问题是，剩下这些需要再测测，字母大小写、名字需要先声明再引用、引用时需考虑作用域范围，函数参数个数及类型、不同作用域的同名变量的作用域范围，常量不能在程序中以任何方式改变其值等方面。
//*01011208 此时的问题是函数中有变量同名的问题没解决。主要涉及entervariable
using namespace std;
template<typename T>
set<T> _union(const set<T> &s1, const set<T> &s2) {
	set<T> res(s1);
	res.insert(s2.begin(), s2.end());
	return res;
}


const int nkw = 18,
alng = 100,
llng = 121,
emax = 322,
emin = -292,
kmax = 15,
tmax = 100,
bmax = 20,
amax = 30,
c2max = 20,
csmax = 30,
cmax = 800,
lmax = 20,
smax = 10000,
ermax = 58,
omax = 63,
xmax = 100000000,
nmax = 100000000,
lineleng = 10000,
linelimit = 200,
stacksize = 2000;

//类型声明
typedef enum {
	intcon, realcon, charcon, stringcon,
	notsy, _plus, _minus, times, idiv, rdiv, imod, andsy, orsy,
	eql, neq, gtr, geq, lss, leq,
	lparent, rparent, lbrack, rbrack, comma, semicolon, period,
	colon, becomes, constsy, typesy, varsy, funcsy,
	procsy, arraysy, recordsy, programsy, ident,
	beginsy, ifsy, casesy, repeatsy, whilesy, forsy,
	endsy, elsesy, untilsy, ofsy, dosy, tosy, downtosy, thensy
}symbol;
char *symbolStr[100] = { "intcon","realcon","charcon","stringcon",
"notsy","_plus","_minus","times","idiv","rdiv","imod","andsy","orsy",
"eql","neq","gtr","geq","lss","leq",
"lparent","rparent","lbrack","rbrack","comma","semicolon","period",
"colon","becomes","constsy","typesy","varsy","funcsy",
"procsy","arraysy","recordsy","programsy","ident",
"beginsy","ifsy","casesy","repeatsy","whilesy","forsy",
"endsy","elsesy","untilsy","ofsy","dosy","tosy","downtosy","thensy" };
typedef int index;// = -xmax..+xmax;
typedef char alfa[alng + 1];// = packed array[1..alng] of char;
typedef enum { konstant, vvariable, typel, prozedure, functionn }objecttyp;
char* objstr[10] = { "const","var","type","pro","func" };
typedef enum { notyp, ints, reals, bools, chars, arrays, records }types;
char*typesstr[10] = { "null","int","real","bool","char","array","record" };
typedef set<symbol> symset;// = set of symbol;
typedef set<types> typset;// = set of types;
typedef struct {
	int i;
	double r;
	types tp;
}conrec;
typedef struct {
	types typ;
	index _ref;
}item;
typedef struct {
	int f, x, y;
}order;
//全局变量
char ch;
double rnum;
int inum,
sleng,
cc,
lc,
ll,
errpos,
t, a, b, sx, c1, c2;
bool iflag, oflag, skipflag, stackdump, prtables;
symbol sy;//: symbol;
set<int> errs;//: set of 0..ermax;
alfa id, progname;
typset stantyps;//: typset;
symset constbegsys, typebegsys, blockbegsys, facbegsys, statbegsys;//: symset;
char line[llng];//: array[1..llng] of char;
alfa key[nkw] = { "array","begin","const",
"do","downto","else","end",
"for","function","if","not",
"of","procedure",
"repeat","then","to","until",
"var" };//: array[1..nkw] of alfa;
symbol ksy[nkw] = { arraysy,beginsy,constsy,
dosy,downtosy,elsesy,endsy,
forsy,funcsy,ifsy,notsy,
ofsy,procsy,
repeatsy,thensy,tosy,untilsy,varsy
};//: array[1..nkw] of symbol;
symbol sps[128];//: array[char] of symbol;
int display[lmax + 1];//: array[0..lmax] of integer;
struct {
	alfa name;
	index link;
	objecttyp obj;
	types typ;
	index _ref;
	bool normal;
	int lev;
	int adr;
}tab[tmax];
struct {
	types inxtyp, eltyp;
	index elref, low, high, elsize, _size;
}atab[amax];
struct {
	index last, lastpar, psize, vsize;
}btab[bmax];
char stab[smax];
double rconst[c2max];
order code[cmax + 1];
FILE *psin, *psout, *prr, *prd, *pstable;
string inf, outf, fprr;
void block(symset fsys, bool isfun, int level);
void statement(symset fsys, int level);
void expression(symset fsys, int level, item * x);

void errormsg()
{
	int k;
	alfa msg[ermax + 1] = { "该标识符未定义","标识符重复定义","应是标识符","程序必须以program 开始","应是'）'         ",
		"应是':' ","非法符号    ","形式参数表中应以标识符,var开头","应是of        ","应是'('         ",
		"类型定义必须是类型","应是[         ","应是]         ","应是..      ","应是;         ",
		"函数结果类型不对","应是=         ","应是条件   ","for循环中循环变量应是integer或char","for循环中初值、终值和循环变量应该相同      ",
		"prog.param","数太大","应是.         ","函数过程声明参数表应有括号","非法字符",
		"常量定义中等号后面必须是常数","下标类型不对","数组声明时，下标应为整型,且应大于零","没有这样的数组","应该为类型标识符",
		"undef type","no record ","调用函数或过程没有参数表的(","算数表达式类型不合法","操作数应该是整型",
		"相比较对象必须相同类型","实参形参应该类型相同","应是变量","string    ","实参个数与形参个数不符",
		"小数点后没数字","read或write参数类型不准确","该表达式应为实型","integer   ","表达式中不能出现过程标识符",
		"应是变量、函数、过程标识符","赋值类型不同 ","表达式中不能出现字符 ","类型不准确","内存溢出",
		"应是常量  ","应是:=        ","应是then      ","应是until     ","应是do        ",
		"应是to downto ","应是begin     ","应是end       ","因子必须以标识符，常量，（开始" };

	fprintf(psout, "\nkey words\n");
	k = 0;
	while (!errs.empty())
	{
		while (errs.find(k) == errs.end())
			k = k + 1;
		fprintf(psout, "%d %s\n", k, msg[k]);
		errs.erase(k);
	}
}

void endskip()
{
	while (errpos < cc)
	{
		fprintf(psout, "_");
		errpos = errpos + 1;
	}
	skipflag = false;
}

void nextch()
{
	static int staticnum = 0;
	if (cc == ll)
	{
		if (feof(psin))
		{
			staticnum++;
			if (staticnum == 2) {
				fprintf(psout, "\nprogram incomplete\n");
				printf("\nprogram incomplete\n");
				errormsg();
				system("pause");
				exit(0);
			}
			return;

		}
		if (errpos != 0)
		{
			if (skipflag)
				endskip();
			fprintf(psout, "\n");
			errpos = 0;
		}
		ll = 0;
		cc = 0;
		fgets(line, llng, psin);
		ll = strlen(line);
		ll++;
		for (int i = 0;i < strlen(line);i++)
			if (line[i] == '\t')
				line[i] = ' ';
		fprintf(psout, "%5d %s", lc, line);
	}
	ch = line[cc];
	cc++;

}
void error(int n)
{
	if (errpos == 0)
		fprintf(psout, "****");
	if (cc > errpos)//这句话使紧邻连续的错误不报出
	{
		for (int i = 0; i < cc - errpos;i++)//这里cc是错误位置
		{
			fputc(' ', psout);
		}
		fprintf(psout, "^%3d", n);
		errpos = cc + 4;
		errs.insert(n);
	}
}

void fatal(int n)
{
	alfa msg[8] = { "", "identifier", "procedures", "reals     ", "arrays    ", "levels    ",
		"code      ", "strings   " };
	fprintf(psout, "\n");
	errormsg();
	fprintf(psout, "compiler table for %s is too small\n", msg[n]);
	exit(0);
}

void insymbol()
{
	int i, j, k, e;
	while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\0')
		nextch();
	if ('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z')
	{
		k = 0;
		do {
			if (k<alng)
			{
				id[k++] = ch;
			}
			nextch();
		} while ('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || '0' <= ch && ch <= '9');
		id[k] = '\0';
		i = 0;
		j = nkw - 1;
		do {
			k = (i + j) >> 1;
			if (strcmp(id, key[k]) <= 0)
			{
				j = k - 1;
			}
			if (strcmp(id, key[k]) >= 0)
			{
				i = k + 1;
			}
		} while (i <= j);
		if (i - 1 > j)
		{
			sy = ksy[k];
		}
		else
		{
			sy = ident;
		}
		//fprintf(psout,"%s\t%s\n",id,symbolStr[sy]);
	}
	else if ('0' <= ch && ch <= '9')
	{
		k = 0;
		inum = 0;
		sy = intcon;
		do {
			inum = inum * 10 + ch - '0';
			k++;
			nextch();
		} while ('0' <= ch && ch <= '9');
		if (k > kmax || inum > nmax)
		{
			error(21);
			inum = k = 0;
		}
		if (ch == '.')
		{
			nextch();
			
			sy = realcon;
			rnum = 0;
			e = 0;
			double dotnum = 0.1;
			while ('0' <= ch && ch <= '9')
			{
				e--;
				rnum = rnum + dotnum * (ch - '0');
				dotnum /= 10;
				nextch();
			}
			if (e == 0)
				error(40);
			if (e < emin)
				rnum = 0;
			rnum = inum + rnum;

		}
		/* if(sy == intcon)
		fprintf(psout,"%d\t%s\n",inum,symbolStr[sy]);
		else
		fprintf(psout,"%lf\t%s\n",rnum,symbolStr[sy]);*/
	}
	else if (ch == ':')
	{
		nextch();
		if (ch == '=')
		{
			sy = becomes;
			nextch();
			//fprintf(psout,":=\t%s\n",symbolStr[sy]);
		}
		else {
			sy = colon;
			// fprintf(psout,":\t%s\n",symbolStr[sy]);
		}
	}
	else if (ch == '<')
	{
		nextch();
		if (ch == '=')
		{
			sy = leq;
			nextch();
			// fprintf(psout,"<=\t%s\n",symbolStr[sy]);
		}
		else if (ch == '>')
		{
			sy = neq;
			nextch();
			// fprintf(psout,"<>\t%s\n",symbolStr[sy]);
		}
		else {
			sy = lss;
			// fprintf(psout,"<\t%s\n",symbolStr[sy]);
		}
	}
	else if (ch == '>')
	{
		nextch();
		if (ch == '=')
		{
			sy = geq;
			nextch();
			//  fprintf(psout,">=\t%s\n",symbolStr[sy]);
		}
		else
		{
			sy = gtr;
			//  fprintf(psout,"<\t%s\n",symbolStr[sy]);
		}
	}
	else if (ch == '.')
	{
		nextch();
		/*if (ch == '.')
		{
			sy = colon;//为什么要是冒号，奇怪
			nextch();
		}
		else
		{*/
		sy = period;
			//   fprintf(psout,".\t%s\n",symbolStr[sy]);
		//}
	}
	else if (ch == '\'')
	{
		nextch();
		if (ch == '\'')
		{
			error(38);
			nextch();
		}
		else if ('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || '0' <= ch && ch <= '9')
		{
			inum = ch;
			sy = charcon;
			nextch();
			if (ch == '\'')
			{
				nextch();
				//         fprintf(psout,"%c\t%s\n",inum,symbolStr[sy]);
			}
			else
			{
				error(24);
			}
		}
		else
		{
			error(24);
		}
	}
	else if (ch == '"')
	{
		k = 0;
		nextch();
		while (ch != '"' && ch >= 32)
		{
			if (sx + k == smax)
			{
				fatal(7);
			}
			stab[sx + k] = ch;
			k++;
			nextch();
		}
		if (ch == '"')
		{
			sy = stringcon;
			inum = sx;
			sleng = k;
			sx = sx + k;
			nextch();
			//     fprintf(psout,"%s\t%s\n",stab+inum,symbolStr[sy]);
		}
		else
		{
			sy = stringcon;
			inum = sx;
			error(59);//字符串中出现非法字符
		}
	}
	else if (ch == '(')
	{
		nextch();
		sy = lparent;
		//   fprintf(psout,"(\t%s\n",symbolStr[sy]);
	}
	else if (ch == '{')
	{
		while (ch != '}')
		{
			nextch();
		}
		nextch();
	}
	else
	{
		switch (ch)
		{
		case '+':case '-':case '*':case '/':case ')':case'=':case',':case'[':case']':case';':
			sy = sps[ch];
			//         fprintf(psout,"%c\t%s\n",ch,symbolStr[sy]);
			nextch();
			break;
		default:nextch();
		}
	}
}

void setup()
{
	sps['+'] = _plus;
	sps['-'] = _minus;
	sps['*'] = times;
	sps['/'] = rdiv;
	sps['('] = lparent;
	sps[')'] = rparent;
	sps['='] = eql;
	sps[','] = comma;
	sps['['] = lbrack;
	sps[']'] = rbrack;
	sps['\''] = neq;
	sps['!'] = andsy;
	sps[';'] = semicolon;
	lc = ll = cc = errpos = 0;
	ch = ' ';
	t = -1;
	a = 0;
	b = 1;
	sx = 0;
	c2 = 0;
	display[0] = 1;
	iflag = oflag = skipflag = prtables = stackdump = false;
}
void enter(alfa x0, objecttyp x1, types x2, int x3)
{
	t++;
	strcpy(tab[t].name, x0);
	tab[t].link = t - 1;
	tab[t].obj = x1;
	tab[t].typ = x2;
	tab[t]._ref = 0;
	tab[t].normal = true;
	tab[t].lev = 0;
	tab[t].adr = x3;
}
void enterarray(int h)
{
	int l = 0;
	types tp = ints;
	if (l > h)
		error(27);
	if (l > xmax || -l > xmax || h > xmax || -h > xmax)
	{
		error(27);
		l = h = 0;
	}
	if (a == amax)
	{
		fatal(4);
	}
	else
	{
		a++;
		atab[a].inxtyp = tp;
		atab[a].low = l;
		atab[a].high = h;
	}
}
void enterblock()
{
	if (b == bmax)
		fatal(2);
	else
	{
		b++;
		btab[b].last = 0;//......
		btab[b].lastpar = 0;
	}
}
void enterreal(double x)
{
	if (c2 == c2max - 1)
	{
		fatal(3);
	}
	else
	{
		rconst[c2 + 1] = x;
		c1 = 1;
		while (rconst[c1] != x)
			c1++;
		if (c1 > c2)
			c2 = c1;
	}
}

void emit2(int fct, int a, int b)
{
	if (lc == cmax)
	{
		fatal(6);
	}
	else
	{
		code[lc].f = fct;
		code[lc].y = b;
		code[lc].x = a;
	}
	lc++;
}
void emit1(int fct, int b)
{
	if (lc == cmax)
	{
		fatal(6);
	}
	else
	{
		code[lc].f = fct;
		code[lc].y = b;
	}
	lc++;
}

void emit(int fct)
{
	if (lc == cmax)
	{
		fatal(6);
	}
	else
	{
		code[lc].f = fct;
	}
	lc++;
}

void printtables()
{
	char mne[omax + 1][5] = { "LDA","LOD","LDI","DIS","",
		"","","","FCT","INT",
		"JMP","JPC","SWT","CAS","F1U",
		"F2U","F1D","F2D","MKS","CAL",
		"IDX","IXX","LDB","CPB","LDC",
		"LDR","FLT","RED","WRS","WRW",
		"WRU","HLT","EXP","EXF","LDT",
		"NOT","MUS","WRR","STO","EQR",
		"NER","LSR","LER","GTR","GER",
		"EQL","NEQ","LSS","LEQ","GRT",
		"GEQ","ORR","ADD","SUB","ADR",
		"SUR","AND","MUL","DIV","MOD",
		"MUR","DIR","RDL","WRT"
	};
	fprintf(psout, "\n\n\n\tident\tlink\tobj\ttyp\tref\tnrm\tlev\tadr\n");
	//for(int i = btab[1].last; i <= t; i++)
	for (int i = 0; i <= t; i++)
	{
		fprintf(psout, "%10d\t%10s\t%10d\t%10s\t%10s\t%10d\t%10d\t%10d\t%10d\n\n",
			i, tab[i].name, tab[i].link, objstr[tab[i].obj],
			typesstr[tab[i].typ], tab[i]._ref,
			(int)(tab[i].normal), tab[i].lev, tab[i].adr);
	}
	fprintf(psout, "\n\nblocks     last    lpar    psize    vsize\n");
	for (int i = 0;i <= b;i++)
	{
		fprintf(psout, "%10d%10d%10d%10d%10d\n",
			i, btab[i].last, btab[i].lastpar, btab[i].psize, btab[i].vsize);
	}
	fprintf(psout, "\n\narray    xtyp   etyp   high  elsz    size\n");
	for (int i = 0;i <= a;i++)
	{
		fprintf(psout, "%4d%9d%5d%5d%5d%5d\n",
			i, atab[i].inxtyp, atab[i].eltyp
			, atab[i].high, atab[i].elsize, atab[i]._size);
	}
	fprintf(psout, "\n\nstring \n%s", stab);

	fprintf(psout, "\n\nrcount\n");
	for (int i = 0;i<10;i++)
		fprintf(psout, "%lf\n", rconst[i]);

	fprintf(psout, "\n\ndisplay\n");
	for (int i = 0;i<10;i++)
		fprintf(psout, "%d\n", display[i]);

	fprintf(psout, "\n\ncode\n");
	for (int i = 0;i<lc;i++)
	{
		if (code[i].f > 30)
			fprintf(psout, "%4d%8s\n", i, mne[code[i].f]);
		else if (code[i].f>7)
			fprintf(psout, "%4d%8s%8d\n", i, mne[code[i].f], code[i].y);
		else
			fprintf(psout, "%4d%8s%8d%8d\n", i, mne[code[i].f], code[i].x, code[i].y);
	}


}
void enterids()
{
	enter(" ", prozedure, notyp, 0);
	enter("real", typel, reals, 1);
	enter("char", typel, chars, 1);
	enter("integer", typel, ints, 1);
	enter("read", prozedure, notyp, 1);
	enter("write", prozedure, notyp, 3);
	enter(" ", prozedure, notyp, 0);
}



void skip(symset fsys, int n)
{
	error(n);
	skipflag = true;
	while (fsys.find(sy) == fsys.end())
	{
		insymbol();
	}
	if (skipflag) {
		endskip();
		skipflag = false;
	}
}

void test(symset s1, symset s2, int n)
{

	if (s1.find(sy) == s1.end())
	{
		s1.insert(s2.begin(), s2.end());
		skip(s1, n);
	}
	return;
}

void testsemicolon(symset fsys)
{
	if (sy == semicolon)
	{
		insymbol();
	}
	else
	{
		error(14);
		if (sy == comma || sy == colon)
		{
			insymbol();
		}
		symset s1;
		s1.insert(ident);
		s1.insert(blockbegsys.begin(), blockbegsys.end());
		test(s1, fsys, 6);
	}
}

void enter(alfa id, objecttyp k, int level)
{
	int j, l;
	if (t == tmax)
	{
		fatal(1);
	}
	else
	{
		strcpy(tab[0].name, id);
		j = btab[display[level]].last;
		l = j;

		//下面只是判断该层有没有同名变量
		bool flag = true;
		int thisLev = level;
		while (strcmp(tab[j].name, id) != 0)
		{
			j = tab[j].link;
		}
		if (j != 0) {
			flag = false;
			//error(1);
		}
		else {
			thisLev--;
			if (thisLev >= 1) {
				j = btab[display[thisLev]].last;
				if (strcmp(tab[j].name, id) == 0) {
					flag = false;
				}
				thisLev--;
			}
		}
		if (flag == false)
			error(1);
		else
		{
			t++;
			strcpy(tab[t].name, id);
			tab[t].link = l;
			tab[t].obj = k;
			tab[t].typ = notyp;
			tab[t]._ref = level;
			tab[t].lev = level;
			tab[t].adr = 0;
			tab[t].normal = false;
		}
		btab[display[level]].last = t;
	}
}

int loc(alfa id, int level)
{//如果没找到会报错。声明变量时不用这个函数找，因为那里找不到是对的。
	int i, j;
	i = level;
	strcpy(tab[0].name, id);
	do {
		j = btab[display[i]].last;
		//下面循环是从该层最后往前找，如果找到该层最前部分，则j=0，则找到了。退出里层循环
		while (strcmp(tab[j].name, id) != 0)
			j = tab[j].link;
		i--;
	} while (i >= 0 && j == 0);
	//如果j==0说明该层没找到，需要向上一层找；如果j!=0，说明找到了

	if (j == 0)
	{
		error(0);
	}
	return j;
}

void entervariable(int level)
{
	if (sy == ident)
	{
		enter(id, vvariable, level);
		insymbol();
	}
	else
	{
		error(2);
	}
}

void constant(symset fsys, conrec * c, int level)
{
	int  sign;
	c->tp = notyp;
	c->i = 0;
	test(constbegsys, fsys, 50);
	if (constbegsys.find(sy) != constbegsys.end())
	{
		if (sy == charcon)
		{
			c->tp = chars;
			c->i = inum;
			insymbol();
		}
		else
		{
			sign = 1;
			if (sy == _plus || sy == _minus)
			{
				if (sy == _minus)
					sign = -1;
				insymbol();
			}

			if (sy == intcon)
			{
				c->tp = ints;
				c->i = sign * inum;
				insymbol();
				//fprintf(psout,"this is a int const\n");
			}
			else if (sy == realcon)
			{
				c->tp = reals;
				c->r = sign * rnum;
				insymbol();
			}
			else
			{
				skip(fsys, 50);
			}
		}
		symset nu;
		test(fsys, nu, 6);
	}
}

void typ(symset fsys, types * tp, int * rf, int * sz, int level)
{
	int  x;
	*tp = notyp;
	*rf = *sz = 0;
	test(typebegsys, fsys, level);
	if (typebegsys.find(sy) != typebegsys.end())
	{
		if (sy == ident)
		{
			x = loc(id, level);
			if (x != 0)//如果x=0，则已经报错
			{
				if (tab[x].obj != typel)
					error(29);
				else
				{
					//以下是基本类型的值，adr表示大小
					*tp = tab[x].typ;
					*rf = tab[x]._ref;
					*sz = tab[x].adr;
					if (*tp == notyp)
						error(30);
				}
			}
			
			insymbol();
		}
		else if (sy == arraysy)
		{
			insymbol();
			if (sy == lbrack)
			{
				insymbol();
			}
			else
			{
				error(11);
				if (sy == lparent)//一个自作多情的判断
					insymbol();
			}
			*tp = arrays;

			types eltp;
			int high;
			int elrf, elsz;
			if (sy == intcon)
			{
				high = inum;
				if (high <= 0)
					error(27);
				enterarray(high);
				*rf = a;
				insymbol();
				if (sy == rbrack) {
					insymbol();
				}
				else {
					error(12);
					if (sy == lparent)//一个自作多情的判断
						insymbol();
				}
				if (sy == ofsy)
					insymbol();
				else error(8);
				typ(fsys, &eltp, &elrf, &elsz, level);
				*sz = high * elsz;
				atab[*rf]._size = *sz;
				atab[*rf].eltyp = eltp;
				atab[*rf].elref = elrf;
				atab[*rf].elsize = elsz;

			}
			else
				error(50);//YINGGAI SHI ZHENGSHU
		}
	}
}

void constdec(symset fsys, int level)
{
	conrec c;
	insymbol();
	symset s1;
	s1.insert(ident);
	test(s1, blockbegsys, 2);
	if (sy == ident)
	{
		enter(id, konstant, level);
		insymbol();
		if (sy == eql)
		{
			insymbol();
		}
		else
		{
			error(16);
			if (sy == becomes)
				insymbol();
		}
		symbol arr[] = { semicolon,comma,ident };
		symset s2(arr, arr + 3);
		s2.insert(fsys.begin(), fsys.end());
		constant(s2, &c, level);
		tab[t].typ = c.tp;
		tab[t]._ref = 0;
		if (c.tp == reals)
		{
			enterreal(c.r);
			tab[t].adr = c1;
		}
		else
		{
			tab[t].adr = c.i;
		}
	}
	while (sy == comma)
	{
		insymbol();
		if (sy == ident)
		{
			enter(id, konstant, level);
			insymbol();
			if (sy == eql)
			{
				insymbol();
			}
			else
			{
				error(16);
				if (sy == becomes)
					insymbol();
			}
			symbol arr[] = { semicolon,comma,ident };
			symset s2(arr, arr + 3);
			s2.insert(fsys.begin(), fsys.end());
			constant(s2, &c, level);
			tab[t].typ = c.tp;
			tab[t]._ref = 0;
			if (c.tp == reals)
			{
				enterreal(c.r);
				tab[t].adr = c1;
			}
			else
			{
				tab[t].adr = c.i;
			}
		}
		else error(2);

	}
	//fprintf(psout,"this is a constdec\n");
	testsemicolon(fsys);

}

void variabledeclaration(symset fsys, int level, int *dx)
{
	types tp;
	int t0, t1, rf, sz;
	insymbol();
	while (sy == ident)
	{
		t0 = t;
		entervariable(level);
		while (sy == comma)
		{
			insymbol();
			entervariable(level);
		}
		if (sy == colon)
			insymbol();
		else error(5);
		t1 = t;
		symbol arr[] = { semicolon, comma, ident };
		symset s1(arr, arr + 3);
		s1.insert(fsys.begin(), fsys.end());
		typ(s1, &tp, &rf, &sz, level);//读取这些变量的声明的类型
		while (t0 < t1)
		{
			t0++;
			tab[t0].typ = tp;
			tab[t0]._ref = rf;
			tab[t0].lev = level;
			tab[t0].adr = *dx;
			tab[t0].normal = true;
			(*dx) += sz;
		}
		testsemicolon(fsys);
	}
	//fprintf(psout,"this is a vardec!\n");
}

void procedeclaration(symset fsys, int level)
{
	bool isfun;
	isfun = (sy == funcsy);
	insymbol();
	if (sy != ident)
	{
		error(2);
		strcpy(id, "");
	}
	if (isfun)
		enter(id, functionn, level);
	else
		enter(id, prozedure, level);
	tab[t].normal = true;
	insymbol();
	fsys.insert(semicolon);
	block(fsys, isfun, level + 1);
	if (sy == semicolon)
	{
		insymbol();
	}
	else error(14);
	emit(32 + (int)isfun);
	//fprintf(psout,"this is a procedeclaration or funcdec\n");
}

void parameterlist(symset fsys, int level, int * dx)
{
	types tp;
	bool valpar;
	int rf, sz, x, t0;
	insymbol();
	tp = notyp;
	rf = sz = 0;
	symset s1, s2;
	s1.insert(ident);
	s1.insert(varsy);
	s1.insert(rparent);
	s2.insert(rparent);
	s2.insert(fsys.begin(), fsys.end());
	test(s1, s2, 7);
	while (sy == ident || sy == varsy)
	{
		if (sy != varsy)
		{
			valpar = true;
		}
		else
		{
			insymbol();
			valpar = false;
		}
		t0 = t;//zhi hou tab[t0]yaogai typ
		entervariable(level);
		while (sy == comma)
		{
			insymbol();
			entervariable(level);
		}
		if (sy == colon)
		{
			insymbol();
			if (sy != ident)
			{
				error(2);
			}
			else

			{
				x = loc(id, level);
				insymbol();
				if (x != 0)
				{
					if (tab[x].obj == typel)
					{
						tp = tab[x].typ;
						rf = tab[x]._ref;
						if (valpar)
							sz = tab[x].adr;
						else sz = 1;
					}
					else error(29);
				}
			}
			symset s3, s4;
			s3.insert(semicolon);
			s3.insert(rparent);
			s4.insert(comma);
			s4.insert(ident);
			s4.insert(fsys.begin(), fsys.end());
			test(s3, s4, 14);
		}
		else error(5);
		while (t0 < t)
		{
			t0++;
			tab[t0].typ = tp;
			tab[t0]._ref = rf;
			tab[t0].adr = *dx;
			tab[t0].lev = level;
			tab[t0].normal = valpar;
			*dx = *dx + sz;

		}
		if (sy != rparent)
		{
			if (sy == semicolon)
			{
				insymbol();
			}
			else
			{
				error(14);
				if (sy == comma)
					insymbol();
			}

			symset s5, s6;
			s5.insert(ident);
			s5.insert(varsy);
			s6.insert(rparent);
			s6.insert(fsys.begin(), fsys.end());
			test(s5, s6, 6);
		}
	}
	if (sy == rparent)
	{
		insymbol();
		symset s7;
		s7.insert(semicolon);
		s7.insert(colon);
		test(s7, fsys, 6);
	}
	else error(4);
}

void block(symset fsys, bool isfun, int level)
{
	int dx, prt, prb, x;
	dx = 5;
	prt = t;
	if (level > lmax)
	{
		fatal(5);
	}
	symbol arr[] = { lparent, colon, semicolon };
	symset s1(arr, arr + 3);
	test(s1, fsys, 14);
	enterblock();//里面b增加1，且指向当前的block
	prb = b;
	display[level] = b;//本程序认为待编译程序中第一行代码属于下标为2的block
	tab[prt].typ = notyp;//往tab中标记本分程序的信息
	tab[prt]._ref = prb;//
	symset s13;
	s13.insert(lparent);
	test(s13, s1, 23);
	if (sy == lparent)
		parameterlist(fsys, level, &dx);
	btab[prb].lastpar = t;
	btab[prb].psize = dx;
	if (isfun)
	{//if func then have return
		if (sy == colon)
		{
			insymbol();
			if (sy == ident)
			{
				x = loc(id, level);
				insymbol();
				if (x != 0)
				{
					if (stantyps.find(tab[x].typ) != stantyps.end())

						tab[prt].typ = tab[x].typ;
					else error(15);
				}
			}
			else
			{
				symset s1;
				s1.insert(semicolon);
				s1.insert(fsys.begin(), fsys.end());
				skip(s1, 2);
			}
		}
		else error(5);
	}

	if (sy == semicolon)
		insymbol();
	else error(14);
	if (sy == constsy)
		constdec(fsys, level);
	if (sy == varsy) {
		variabledeclaration(fsys, level, &dx);
	}
	btab[prb].vsize = dx;
	//btab[prb].last=t;
	while (sy == procsy || sy == funcsy)
	{
		procedeclaration(fsys, level);
	}

	symset s6, s2;
	s6.insert(beginsy);
	s2.insert(blockbegsys.begin(), blockbegsys.end());
	s2.insert(statbegsys.begin(), statbegsys.end());
	test(s6, s2, 56);

	tab[prt].adr = lc;
	insymbol();
	symbol arr1[] = { endsy,semicolon };
	symset s10(arr1, arr1 + 2), s11;
	s11 = _union(s10, fsys);;
	statement(s11, level);
	symset s3;
	s3.insert(semicolon);
	s3.insert(statbegsys.begin(), statbegsys.end());
	while (s3.find(sy) != s3.end())
	{
		if (sy == semicolon)
			insymbol();
		else error(14);
		statement(s11, level);
	}
	if (sy == endsy)
		insymbol();
	else error(57);
	symset s4;
	fsys.insert(period);
	test(fsys, s4, 6);
}

void startblock(symset fsys, bool isfun, int level)
{
	int dx, prt, prb;
	dx = 5;
	prt = t;
	if (level > lmax)
	{
		fatal(5);
	}
	enterblock();
	prb = b;
	display[level] = b;
	tab[prt].typ = notyp;
	tab[prt]._ref = prb;
	btab[prb].lastpar = t;
	btab[prb].psize = dx;


	if (sy == constsy)
		constdec(fsys, level);
	if (sy == varsy) {
		variabledeclaration(fsys, level, &dx);

	}
	btab[prb].vsize = dx;
	while (sy == procsy || sy == funcsy)
	{
		procedeclaration(fsys, level);
	}

	symset s6, s2;
	s6.insert(beginsy);
	s2.insert(blockbegsys.begin(), blockbegsys.end());
	s2.insert(statbegsys.begin(), statbegsys.end());
	test(s6, s2, 56);

	tab[prt].adr = lc;
	insymbol();

	symbol arr1[] = { endsy,semicolon };
	symset s10(arr1, arr1 + 2), s11;
	s11 = _union(s10, fsys);;
	statement(s11, level);
	symset s3;
	s3.insert(semicolon);
	s3.insert(statbegsys.begin(), statbegsys.end());
	while (s3.find(sy) != s3.end())
	{
		if (sy == semicolon)
			insymbol();
		else error(14);
		statement(s11, level);
	}
	if (sy == endsy)
		insymbol();
	else error(57);
	symset s4;
	fsys.insert(period);
	test(fsys, s4, 6);
}

void selector(symset fsys, int level, item * v)
{
	item x;
	if (sy == lbrack)
	{
		insymbol();
		symbol arr[] = { comma,rbrack };
		symset s1(arr, arr + 2), s2;
		s2 = _union(s1, fsys);
		expression(s2, level, &x);
		if (v->typ != arrays)
		{
			error(28);
		}
		else {
			int a = v->_ref;
			if (atab[a].inxtyp != x.typ)//此时左边的值恒为ints
			{
				error(26);
			}
			else if (atab[a].elsize == 1)//永远成立
			{
				emit1(20, a);
			}
			//else emit1(21, a);
			v->typ = atab[a].eltyp;
			v->_ref = atab[a].elref;
		}
	}
	if (sy == rbrack)
	{
		insymbol();
	}
	else
	{
		error(12);
	}
	symset nu;
	symset s1;
	s1.insert(becomes);
	s1.insert(eql);
	s1.insert(fsys.begin(), fsys.end());
	test(s1, nu, 6);
}

void assignment(int lv, int ad, int i, symset fsys, int level)
{
	item x, y;
	int f;
	x.typ = tab[i].typ;
	x._ref = tab[i]._ref;
	if (tab[i].normal)
	{
		f = 0;
	}
	else f = 1;
	emit2(f, lv, ad);
	if (sy == lbrack) {
		symbol arr[] = { becomes,eql };
		symset s1(arr, arr + 2);
		s1.insert(fsys.begin(), fsys.end());
		selector(s1, level, &x);
	}



	if (sy == becomes)
		insymbol();
	else {
		error(51);
		if (sy == eql)
			insymbol();
	}

	expression(fsys, level, &y);
	if (x.typ == y.typ)
	{
		if (stantyps.find(x.typ) != stantyps.end())
			emit(38);
		else if (x._ref != y._ref)
			error(46);
		else if (x.typ == arrays)
			emit1(23, atab[x._ref]._size);
		else emit1(23, btab[x._ref].vsize);
	}
	else if (x.typ == reals && (y.typ == ints || y.typ == chars))
	{
		emit1(26, 0);
		emit(38);
	}
	else if (x.typ == ints && y.typ == chars)
		emit(38);
	else if (x.typ != notyp && y.typ != notyp)error(46);
	//fprintf(psout,"\nthis is an assignment!\n");
}

void compountstatement(symset fsys, int level)
{
	insymbol();
	symbol arr[] = { semicolon,endsy };
	symset s1(arr, arr + 2);
	symset s2 = _union(s1, fsys);
	statement(s2, level);
	while (statbegsys.find(sy) != statbegsys.end() || sy == semicolon)
	{
		if (sy == semicolon)
			insymbol();
		else error(14);
		statement(s2, level);
	}
	if (sy == endsy)
		insymbol();
	else error(57);
	//fprintf(psout,"\nthis is a compountstatement!\n");
}

void ifstatement(symset fsys, int level)
{
	item x;int lc1, lc2;
	insymbol();
	symbol arr[] = { thensy,dosy };
	symset s1(arr, arr + 2), s2;
	s2 = _union(s1, fsys);
	expression(s2, level, &x);
	if (x.typ != bools && x.typ != notyp)
		error(17);
	lc1 = lc;
	emit(11);

	if (sy == thensy)
	{
		insymbol();
	}
	else {
		error(52);
		if (sy == dosy)
		{
			insymbol();
		}
	}
	symset s3, s4;
	s3.insert(elsesy);
	s4 = _union(s3, fsys);
	statement(s4, level);
	if (sy == elsesy)
	{
		insymbol();
		lc2 = lc;
		emit(10);
		code[lc1].y = lc;
		statement(fsys, level);
		code[lc2].y = lc;
	}
	else code[lc1].y = lc;
	//fprintf(psout,"\nthis is a if!\n");
}

void repeatstatement(symset fsys, int level)
{
	item x;
	int lc1;
	lc1 = lc;
	insymbol();
	symbol arr[] = { semicolon,untilsy };
	symset s1(arr, arr + 2);
	statement(_union(s1, fsys), level);
	if (sy == untilsy)
	{
		insymbol();
		expression(fsys, level, &x);
		if (x.typ != bools && x.typ != notyp)
		{
			error(17);
		}
		emit1(11, lc1);
	}
	//fprintf(psout,"\nthis is a repreatstatement!\n");
}

void forstatement(symset fsys, int level)
{
	types cvt;
	item x;
	int i, f, lc1, lc2;
	insymbol();
	if (sy == ident)
	{
		i = loc(id, level);
		insymbol();
		if (i == 0)
			cvt = ints;
		else if (tab[i].obj == vvariable)
		{
			cvt = tab[i].typ;
			if (!tab[i].normal)
				error(37);
			else emit2(0, tab[i].lev, tab[i].adr);
			if (cvt != notyp && cvt != ints && cvt != chars)
				error(18);
		}
		else
		{
			error(37);
			cvt = ints;
		}
	}
	else
	{
		symbol arr[] = { becomes,tosy,downtosy,dosy };
		symset s1(arr, arr + 4);
		s1.insert(fsys.begin(), fsys.end());
		skip(s1, 2);
	}
	if (sy == becomes)
	{
		insymbol();
		symbol arr[] = { tosy,downtosy,dosy };
		symset s1(arr, arr + 3), s2;
		s2 = _union(s1, fsys);
		expression(s2, level, &x);
		if (x.typ != cvt)
			error(19);
	}
	else
	{
		symbol arr[] = { tosy,downtosy,dosy };
		symset s1(arr, arr + 3);
		s1.insert(fsys.begin(), fsys.end());
		skip(s1, 51);
	}
	f = 14;
	if (sy == tosy || sy == downtosy)
	{
		if (sy == downtosy)
		{
			f = 16;
		}
		insymbol();
		symbol arr[] = { dosy };
		symset s1(arr, arr + 1), s2;
		s2 = _union(s1, fsys);
		expression(s2, level, &x);
		if (x.typ != cvt)
			error(19);
	}
	else
	{
		symbol arr[] = { dosy };
		symset s1(arr, arr + 1);
		s1.insert(fsys.begin(), fsys.end());
		skip(s1, 55);
	}
	lc1 = lc;
	emit(f);
	if (sy == dosy)
	{
		insymbol();
	}
	else error(54);
	lc2 = lc;
	statement(fsys, level);
	emit1(f + 1, lc2);
	code[lc1].y = lc;
	//fprintf(psout,"\nthis is a forstatement!\n");
}

void standproc(int n, symset fsys, int level)
{
	int i, f;
	item x;
	switch (n)
	{
	case 1:
		if (sy = lparent)
		{
			do {
				insymbol();
				if (sy != ident) error(2);
				else {
					i = loc(id, level);
					insymbol();
					if (i != 0)
					{
						if (tab[i].obj != vvariable) error(37);
						else
						{
							x.typ = tab[i].typ;
							x._ref = tab[i]._ref;
							if (tab[i].normal)
								f = 0;
							else f = 1;
							emit2(f, tab[i].lev, tab[i].adr);
							if (sy == lbrack) {
								symbol arr[] = { comma,rparent };
								symset s1(arr, arr + 2);
								s1.insert(fsys.begin(), fsys.end());
								selector(s1, level, &x);
							}
							if (x.typ == ints || x.typ == reals
								|| x.typ == chars || x.typ == notyp)
								emit1(27, x.typ);
							else error(41);
						}
					}
				}
			} while (sy == comma);
			if (sy == rparent)insymbol();
			else error(4);
		}
		break;
	case 3:
		if (sy == lparent)
		{
			insymbol();
			if (sy == stringcon)
			{
				emit1(24, sleng);
				emit1(28, inum);
				insymbol();
				if (sy == comma)
				{
					insymbol();
					symbol arr[] = { comma, colon, rparent };
					symset s1(arr, arr + 3), s2;
					s2 = _union(s1, fsys);
					expression(s2, level, &x);
					if (stantyps.find(x.typ) == stantyps.end())
					{
						error(41);
					}
					emit1(29, x.typ);
				}
			}
			else {
				symbol arr[] = { comma, colon, rparent };
				symset s1(arr, arr + 3), s2;
				s2 = _union(s1, fsys);
				expression(s2, level, &x);
				if (stantyps.find(x.typ) == stantyps.end())
				{
					error(41);
				}
				emit1(29, x.typ);
			}
			if (sy == rparent)
			{
				insymbol();
			}
			else error(4);
		}

	}
	//fprintf(psout,"\nthis is read or write!\n");
}

void call(symset fsys, int tab_pos, int level)
{//传入的tab_pos为函数或过程在tab表中的位置
	item x;
	int lastp, cp, k;
	emit1(18, tab_pos);//标记栈，开5个空间，倒数第二个空间存vsize-1，栈顶存tab_pos
	lastp = btab[tab[tab_pos]._ref].lastpar;
	cp = tab_pos;//从函数过程名所在下标开始
	if (sy == lparent) {
		if (cp < lastp)
		{
			do {
				insymbol();
				if (cp >= lastp) {//问题是如果传入的参数少会发现吗 还有用原来的pls试试，看他的解决，争取同样的解法。ps1
					error(39);
				}
				else
				{
					cp++;
					if (tab[cp].normal)
					{
						symbol arr[] = { comma, colon, rparent };
						symset s1(arr, arr + 3), s2;
						s2 = _union(s1, fsys);
						expression(s2, level, &x);
						if (x.typ == tab[cp].typ)
						{
							if (x._ref != tab[cp]._ref)
								error(36);
							else if (x.typ == arrays)//这种情况是传入数组，不会发生
								emit1(22, atab[x._ref]._size);
						}
						/*else if (x.typ == ints &&tab[cp].typ == reals)
						{
							emit1(26, 0);
						}
						else if (x.typ == ints && tab[cp].typ == chars ||
							x.typ == chars && tab[cp].typ == ints)
						{
							emit1(22, atab[x._ref]._size);
						}*/
						else if (x.typ != notyp)
							error(36);
					}
					else//by adr
					{
						if (sy != ident)
							error(2);
						else
						{
							k = loc(id, level);
							insymbol();
							if (k != 0)
							{
								if (tab[k].obj != vvariable)//必须是变量名，表达式等都不可以
									error(37);
								x.typ = tab[k].typ;
								x._ref = tab[k]._ref;
								if (tab[k].normal)
									emit2(0, tab[k].lev, tab[k].adr);
								else
									emit2(1, tab[k].lev, tab[k].adr);
								if (sy == lbrack)
								{
									symbol arr[] = { comma,colon,rparent };
									symset s1(arr, arr + 3);
									s1.insert(fsys.begin(), fsys.end());
									selector(s1, level, &x);
								}
								if (x.typ != tab[cp].typ || x._ref != tab[cp]._ref)
									error(36);
							}
						}
					}//else//by adr
				}
				symbol arr1[] = { comma,rparent };
				symset s2(arr1, arr1 + 2);
				test(s2, fsys, 6);
			} while (sy == comma);

		}
		else if (cp == lastp)
			insymbol();
		else if (cp > lastp) {
			error(39);
		}
		if (cp < lastp)
		{
			error(39);//实参少
		}
		if (sy == rparent) insymbol();
		else error(4);
	}
	else
		error(32);

	emit1(19, btab[tab[tab_pos]._ref].psize - 1);
	if (tab[tab_pos].lev < level)
	{
		emit2(3, tab[tab_pos].lev, level);//更新display
	}
	//fprintf(psout,"\nthis is a call!\n");

}

void statement(symset fsys, int level)
{
	int i;
	if (statbegsys.find(sy) != statbegsys.end() || sy == ident)
	{
		switch (sy)
		{
		case ident:
			i = loc(id, level);
			insymbol();
			if (i != 0)
			{
				switch (tab[i].obj)
				{
				case konstant:case typel:error(45);break;
				case vvariable: assignment(tab[i].lev, tab[i].adr, i, fsys, level);break;
				case prozedure:
					if (tab[i].lev != 0)
						call(fsys, i, level);
					else standproc(tab[i].adr, fsys, level);
					break;
				case functionn://ret value
					if (tab[i]._ref == display[level])
						assignment(tab[i].lev + 1, 0, i, fsys, level);
					else
						error(45);
				}
			}
			break;
		case beginsy: compountstatement(fsys, level);break;
		case ifsy: ifstatement(fsys, level);break;
		case repeatsy: repeatstatement(fsys, level);break;
		case forsy: forstatement(fsys, level);break;

		}

	}
	// symset nu;
	//test(fsys,nu,14);
}

types resulttype(types a, types b)
{
	a = (a == chars) ? ints : a;
	b = (b == chars) ? ints : b;
	if (a == ints && b == chars || a == chars && b == ints || a == chars && b == chars)
	{
		return ints;
	}
	else if (a>reals || b > reals)
	{
		error(33);
		return notyp;
	}
	else if (a == notyp || b == notyp)
	{
		return notyp;
	}
	else if (a == ints)
	{
		if (b == ints)
			return ints;
		else {
			emit1(26, 1);
			return reals;
		}
	}
	else
	{
		if (b == ints)
			emit1(26, 0);
		return reals;
	}
}
void factor(symset fsys, int level, item *x)
{
	int i, f;
	x->typ = notyp;
	x->_ref = 0;
	test(facbegsys, fsys, 58);
	if (facbegsys.find(sy) != facbegsys.end())
	{
		if (sy == ident)
		{
			i = loc(id, level);
			insymbol();
			if (i == 0) {
				error(0);
			}
			else {
				switch (tab[i].obj)
				{
				case konstant:
					x->typ = tab[i].typ;
					x->_ref = 0;
					if (x->typ == reals)
					{
						emit1(25, tab[i].adr);
					}
					else emit1(24, tab[i].adr);
					break;
				case vvariable:
					x->typ = tab[i].typ;
					x->_ref = tab[i]._ref;
					if (sy == lbrack)
					{
						f = (tab[i].normal) ? 0 : 1;
						emit2(f, tab[i].lev, tab[i].adr);
						selector(fsys, level, x);
						if (stantyps.find(x->typ) != stantyps.end())
							emit(34);
					}
					else
					{
						if (stantyps.find(x->typ) != stantyps.end())
							f = (tab[i].normal) ? 1 : 2;
						else f = (tab[i].normal) ? 0 : 1;
						emit2(f, tab[i].lev, tab[i].adr);
					}
					break;
				case prozedure:
				//case typel:
					error(44);break;
				case functionn:
					x->typ = tab[i].typ;
					if (tab[i].lev != 0)
					{
						call(fsys, i, level);
					}
					else error(0);
				}
			}
		}
		else if (sy == charcon || sy == intcon || sy == realcon)
		{
			if (sy == realcon)
			{
				x->typ = reals;
				enterreal(rnum);
				emit1(25, c1);
			}
			else
			{
				x->typ = (sy == charcon) ? chars : ints;
				if (sy == charcon)
					error(47);
				emit1(24, inum);
			}
			x->_ref = 0;
			insymbol();
		}
		else if (sy == lparent)
		{
			insymbol();
			symset s1;
			s1.insert(rparent);
			symset s2 = _union(s1, fsys);
			expression(s2, level, x);
			if (sy == rparent)
			{
				insymbol();
			}
			else error(4);
		}
		else if (sy == notyp)
		{
			insymbol();
			factor(fsys, level, x);
			if (x->typ == bools)
			{
				emit(35);
			}
			else if (x->typ != notyp)
			{
				error(32);
			}
		}
		test(fsys, facbegsys, 6);
	}//end while
}

void term(symset fsys, int level, item *x)
{
	item y;
	symbol op;
	symbol arr[] = { times, rdiv, idiv, imod };
	symset s1(arr, arr + 4);
	symset s2 = _union(s1, fsys);
	factor(s2, level, x);
	while (s1.find(sy) != s1.end())
	{
		op = sy;
		insymbol();
		factor(s2, level, &y);
		if (op == times)
		{
			x->typ = resulttype(x->typ, y.typ);
			switch (x->typ)
			{
			case notyp:break;
			case ints:emit(57);break;
			case reals:emit(60);break;
			}
		}
		else if (op == rdiv)
		{
			x->typ = (x->typ == chars) ? ints : x->typ;
			y.typ = (y.typ == chars) ? ints : y.typ;
			if (x->typ == ints && y.typ == ints)
			{
				emit(58);
			}
			else {
				if (x->typ == ints)
				{
					emit1(26, 1);
					x->typ = reals;
				}
				if (y.typ == ints)
				{
					emit1(26, 0);
					y.typ = reals;
				}
				if (x->typ == reals && y.typ == reals)
				{
					emit(61);
				}
				else if (x->typ != notyp && y.typ != notyp)
				{
					error(33);
					x->typ = notyp;
				}
			}
		}
	}
}


void simpleexpression(symset fsys, int level, item * x)
{
	item y;
	symbol op;
	if (sy == _plus || sy == _minus)
	{
		op = sy;
		insymbol();
		symbol arr[] = { _plus,_minus };
		symset s1(arr, arr + 2);
		symset s2 = _union(s1, fsys);
		term(s2, level, x);
		if (x->typ > reals && x->typ != chars)
			error(33);
		else if (op == _minus)
			emit(36);
	}
	else {
		symbol arr[] = { _plus,_minus };
		symset s1(arr, arr + 2), s2;
		s2 = _union(s1, fsys);
		term(s2, level, x);
	}
	while (sy == _plus || sy == _minus)
	{
		op = sy;
		insymbol();
		symbol arr[] = { _plus,_minus };
		symset s1(arr, arr + 2);
		symset s2 = _union(s1, fsys);
		term(s2, level, &y);
		x->typ = resulttype(x->typ, y.typ);
		switch (x->typ)
		{
		case notyp:break;
		case ints:emit((op == _plus) ? 52 : 53);break;
		case reals: emit((op == _plus) ? 54 : 55);break;
		}
	}

}

void expression(symset fsys, int level, item * x)
{
	item y;symbol op;
	symbol arr[] = { eql,neq,lss,leq,gtr,geq };
	symset s1(arr, arr + 6);
	symset s2 = _union(s1, fsys);
	simpleexpression(s2, level, x);
	if (s1.find(sy) != s1.end())
	{
		op = sy;
		insymbol();
		simpleexpression(fsys, level, &y);
		x->typ = (x->typ == chars) ? ints : x->typ;
		y.typ = (y.typ == chars) ? ints : y.typ;
		if ((x->typ == notyp || x->typ == ints ||
			x->typ == bools || x->typ == chars) && x->typ == y.typ)
		{
			switch (op)
			{
			case eql:emit(45);break;
			case neq:emit(46);break;
			case lss:emit(47);break;
			case leq:emit(48);break;
			case gtr:emit(49);break;
			case geq:emit(50);break;
			}
		}
		else
		{
			if (x->typ == ints)
			{
				x->typ = reals;
				emit1(26, 1);
			}
			else if (y.typ == ints)
			{
				y.typ = reals;
				emit1(26, 1);
			}
			if (x->typ == reals && y.typ == reals)
			{
				switch (op)
				{
				case eql:emit(39);break;
				case neq:emit(40);break;
				case lss:emit(41);break;
				case leq:emit(42);break;
				case gtr:emit(43);break;
				case geq:emit(44);break;
				}
			}
			else error(35);
		}

		x->typ = bools;
	}
	//    fprintf(psout,"\nthis is a expression!\n");
}

void interpret()
{
	order ir;
	int pc, top, interpret_b, h1, h2, h3, lncnt, ocnt, chrcnt;
	int fld[5];//fld[0] can not be use
	enum { run, fin, caschk, divchk, inxchk, stkchk, linchk, lngchk, redchk } ps;
	ps = run;
	int display[lmax + 1];
	struct {
		int i;
		double r;
		bool b;
		// char c;
	}s[stacksize];

	s[1].i = s[2].i = 0;
	s[3].i = -1;
	s[4].i = btab[1].last;
	display[0] = 0;
	display[1] = 0;
	top = btab[2].vsize - 1;
	interpret_b = 0;
	pc = tab[s[4].i].adr;
	lncnt = 0;
	ocnt = 0;
	chrcnt = 0;
	ps = run;
	fld[1] = 10;
	fld[2] = 22;
	fld[3] = 10;
	fld[4] = 1;
	do {
		ir = code[pc];
		//if(pc == 92)
		//    printf("92");
		//printf("code %d\n",pc);
		pc++;
		ocnt++;

		switch (ir.f / 10)
		{
		case 0:
		{
			switch (ir.f)
			{
			case 0:
				top++;
				if (top>stacksize) ps = stkchk;
				else s[top].i = display[ir.x] + ir.y;//加载x层y位置的变量的地址
				break;
			case 1:
				top++;
				if (top>stacksize) ps = stkchk;
				else s[top] = s[display[ir.x] + ir.y];//加载x层y位置变量在数据栈中的值
				break;
			case 2:
				top++;
				if (top>stacksize) ps = stkchk;
				else s[top] = s[s[display[ir.x] + ir.y].i];
				break;
			case 3:
				h1 = ir.y;//最大的level
				h2 = ir.x;//要调用函数的level
				h3 = interpret_b;
				do {
					display[h1] = h3;
					h1--;
					h3 = s[h3 + 2].i;
				} while (h1 != h2);
				break;
			case 9:
				s[top].i = s[top].i + ir.y;
				break;
			default:fprintf(prr, "shouldnot\n");

			}
			break;
		}

		case 1:
		{
			int h3, h4;
			switch (ir.f)
			{
			case 10: pc = ir.y;break;
			case 11: if (!s[top].b)pc = ir.y;top--;break;
			case 14:
				h1 = s[top - 1].i;
				if (h1 <= s[top].i)
				{
					s[s[top - 2].i].i = h1;
				}
				else
				{
					top -= 3;
					pc = ir.y;
				}
				break;
			case 15:
				h2 = s[top - 2].i;
				h1 = s[h2].i + 1;
				if (h1 <= s[top].i)
				{
					s[h2].i = h1;
					pc = ir.y;
				}
				else top -= 3;
				break;
			case 16:
				h1 = s[top - 1].i;
				if (h1 >= s[top].i)
				{
					s[s[top - 2].i].i = h1;
				}
				else
				{
					pc = ir.y;
					top -= 3;
				}
				break;
			case 17:
				h2 = s[top - 2].i;
				h1 = s[h2].i - 1;
				if (h1 >= s[top].i)
				{
					s[h2].i = h1;
					pc = ir.y;
				}
				else top -= 3;
				break;
			case 18:
				h1 = btab[tab[ir.y]._ref].vsize;
				if (top + h1>stacksize)ps = stkchk;
				else
				{
					top += 5;
					s[top - 1].i = h1 - 1;//display区第四个位置记录vsize-1
					s[top].i = ir.y;//display区第五个位置记录tabpos
				}
				break;

			case 19:
				h1 = top - ir.y;//y为被调用函数的PSIZE-1，所以h1位被调用函数的数据区首地址,此地址存函数结果
				h2 = s[h1 + 4].i;//获取display区第五个位置，tabpos
				h3 = tab[h2].lev;
				display[h3 + 1] = h1;
				h4 = s[h1 + 3].i + h1;
				s[h1 + 1].i = pc;//display第2个位置
				s[h1 + 2].i = display[h3];//静态链
				s[h1 + 3].i = interpret_b;//动态链，上一层的位置
				for (h3 = top + 1; h3 <= h4; h3++)
				{
					s[h3].i = 0;
				}
				interpret_b = h1;
				top = h4;
				pc = tab[h2].adr;
				break;
			default:fprintf(prr, "shouldnot\n");
			}

			break;
		}
		case 2:
		{
			switch (ir.f)
			{
			case 20:
				h1 = ir.y;
				h2 = atab[h1].low;
				h3 = s[top].i;
				if (h3 < h2)
				{
					ps = inxchk;
				}
				else if (h3 > atab[h1].high)
				{
					ps = inxchk;
				}
				else
				{
					top--;
					s[top].i = s[top].i + h3 - h2;
				}
				break;
			case 22:
				h1 = s[top].i;
				top--;
				h2 = ir.y + top;
				if (h2 > stacksize)
					ps = stkchk;
				else
					while (top < h2)
					{
						top++;
						s[top] = s[h1];
						h1++;
					}
			case 23:
				h1 = s[top - 1].i;
				h2 = s[top].i;
				h3 = h1 + ir.y;
				while (h1 < h3)
				{
					s[h1] = s[h2];
					h1++;h2++;
				}
				top -= 2;
				break;
			case 24:
				top++;
				if (top > stacksize)
					ps = stkchk;
				else s[top].i = ir.y;
				break;
			case 25:
				top++;
				if (top > stacksize)
					ps = stkchk;
				else s[top].r = rconst[ir.y];
				break;
			case 26:
				h1 = top - ir.y;
				s[h1].r = s[h1].i;
				break;
			case 27:
				switch (ir.y)
				{
					
				case 1:scanf("%d", &s[s[top].i].i);break;
				case 2:scanf("%lf", &s[s[top].i].r);break;
				case 4:
					char ch;
					do {
						scanf("%c", &ch);
					} while (!('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || '0' <= ch && ch <= '9'));
					s[s[top].i].i = ch;
					break;
				}
				top--;
				break;
			case 28:
				h1 = s[top].i;
				h2 = ir.y;
				top--;
				chrcnt += h1;
				if (chrcnt>lineleng)
					ps = lngchk;
				do {
					fprintf(prr, "%c", stab[h2]);
					h1--;
					h2++;
				} while (h1 > 0);
				fprintf(prr, "\n");
				break;
			case 29:
				chrcnt += fld[ir.y];
				if (chrcnt > lineleng)
					ps = lngchk;
				switch (ir.y)
				{
				case 1:fprintf(prr, "%d\n", s[top].i);break;
				case 2:fprintf(prr, "%lf\n", s[top].r);break;
				case 4:fprintf(prr, "%c\n", s[top].i);break;
				default:fprintf(prr, "should not \n");
				}
				top--;
				break;
			default:fprintf(prr, "shouldnot\n");
			}
			break;
		}
		case 3:
		{
			switch (ir.f)
			{
			case 30:
				chrcnt += s[top].i;
				if (chrcnt > lineleng)
					ps = lngchk;
				else {
					switch (ir.y)
					{
					case 1:fprintf(prr, "%d", s[top].i);break;
					case 2:fprintf(prr, "%f", s[top].r);break;
					case 4:fprintf(prr, "%c", s[top].i);break;
					default:fprintf(prr, "should not ");
					}
				}
				break;
			case 31://停止
				ps = fin;break;
			case 32://退出过程
				top = interpret_b - 1;
				pc = s[interpret_b + 1].i;
				interpret_b = s[interpret_b + 3].i;
				break;
			case 33://退出函数
				top = interpret_b;
				pc = s[interpret_b + 1].i;
				interpret_b = s[interpret_b + 3].i;
				break;
			case 34://取栈顶单元内容为地址的单元内容
				s[top] = s[s[top].i];break;
			//case 35:s[t].b = !s[t].b;break;
			case 36:s[top].i = -s[top].i;s[top].r = -s[top].r;break;
			case 37:chrcnt += s[top - 1].i;
				fprintf(prr, "%lf", s[top - 2].r);//
				top -= 3;
				break;
			case 38:
				s[s[top - 1].i] = s[top];
				top -= 2;
				break;
			case 39:
				top--;
				s[top].b = (s[top].r == s[top + 1].r);
				break;
			default:fprintf(prr, "shouldnot\n");
			}
			break;
		}
		case 4:
		{
			switch (ir.f)
			{
			case 40:top--;s[top].b = (s[top].r != s[top + 1].r);break;
			case 41:top--;s[top].b = (s[top].r < s[top + 1].r);break;
			case 42:top--;s[top].b = (s[top].r <= s[top + 1].r);break;
			case 43:top--;s[top].b = (s[top].r > s[top + 1].r);break;
			case 44:top--;s[top].b = (s[top].r >= s[top + 1].r);break;
			case 45:top--;s[top].b = (s[top].i == s[top + 1].i);break;
			case 46:top--;s[top].b = (s[top].i != s[top + 1].i);break;
			case 47:top--;s[top].b = (s[top].i < s[top + 1].i);break;
			case 48:top--;s[top].b = (s[top].i <= s[top + 1].i);break;
			case 49:top--;s[top].b = (s[top].i > s[top + 1].i);break;
			}
			break;
		}
		case 5:
		{
			switch (ir.f)
			{
			case 50:top--;s[top].b = (s[top].i >= s[top + 1].i);break;
			case 52:top--;s[top].i += s[top + 1].i;break;
			case 53:top--;s[top].i -= s[top + 1].i;break;
			case 54:top--;s[top].r += s[top + 1].r;break;
			case 55:top--;s[top].r -= s[top + 1].r;break;
			case 57:top--;s[top].i *= s[top + 1].i;break;
			case 58:top--;
				if (s[top + 1].i == 0)
					ps = divchk;
				else s[top].i = s[top].i / s[top + 1].i;
				break;
			/*case 59:
				t--;
				if (s[t + 1].i == 0)
					ps = divchk;
				else s[t].i = s[t].i % s[t + 1].i;
				break;*/
			default:fprintf(prr, "shouldnot\n");

			}
			break;
		}
		case 6:
		{

			switch (ir.f)
			{
			case 60:top--;s[top].r *= s[top + 1].r;break;
			case 61:top--;s[top].r /= s[top + 1].r;break;
			default:fprintf(prr, "shouldnot\n");
			}
		}

		}
	} while (ps == run);
}

int main()
{
	//    errs.insert(1);
	//    int k = 0;
	//    errs.insert(3);
	//    while (!errs.empty()){
	//    while (errs.find(k)==errs.end()) k = k + 1 ;
	//    printf("%d\n" ,k);
	//    errs.erase(k);
	//    }
	setup();

	symbol constbeg[] = { _plus,_minus,intcon,realcon,charcon,ident },
		typebeg[] = { ident,arraysy },
		blockbeg[] = { constsy,varsy,procsy,funcsy,beginsy },
		facbeg[] = { intcon, realcon, charcon, ident, lparent, notsy },
		statbeg[] = { beginsy, ifsy, repeatsy, forsy };
	constbegsys = symset(constbeg, constbeg + 6);
	typebegsys = symset(typebeg, typebeg + 2);
	blockbegsys = symset(blockbeg, blockbeg + 5);
	facbegsys = symset(facbeg, facbeg + 6);
	statbegsys = symset(statbeg, statbeg + 4);
	types stanty[] = { notyp, ints, reals, chars };
	stantyps = typset(stanty, stanty + 5);
	char path[300];
	fgets(path, 300, stdin);
	path[strlen(path) - 1] = 0;
	printf("\n");
	psin = fopen(path, "r");
	if (psin == NULL) {
		printf("file can not find");
		system("pause");
		exit(0);
	}
	psout = stdout;
	psout = fopen("result.txt", "w");
	prr = stdout;
	enterids();
	btab[1].last = t;
	btab[1].lastpar = 1;
	btab[1].psize = 0;
	btab[1].vsize = 0;
	insymbol();
	int dx = 5;
	symset s1, s2;
	s2 = _union(s1, blockbegsys);
	s1 = _union(s2, statbegsys);
	startblock(s1, false, 1);
	if (sy != period)
	{
		error(2);
	}
	else emit(31);

	printtables();
	if (errs.empty()) {
		interpret();
		errormsg();
	}
	else {
		printf("error");
		//psout = stdout;

		errormsg();
	}
	fclose(psout);
	fclose(psin);
	system("pause");
	return 1;
}
