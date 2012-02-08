%union{
	int value;
	char oper;
}

%token <value> INT
%token FILE_EOF
%token '+' '-' '*' '/'
%token '(' ')' '\n'

%type <value> expr

%start start

%left '+' '-'
%left '*' '/'

%%

start:
	| start expr '\n' { printf("%d\n", $2); }
	| start '\n' {puts("0");}
	| FILE_EOF

expr: INT { $$ = $1; }
	| expr '+' expr { $$ = $1 + $3; }
	| expr '-' expr { $$ = $1 - $3; }
	| expr '*' expr { $$ = $1 * $3; }
	| expr '/' expr { $$ = $1 / $3; }
	| '(' expr ')' { $$ = $2; }


