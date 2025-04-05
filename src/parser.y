%{
#include <stdio.h>
#include <string>
#include <vector>
#include "AST.h"

extern int yylex();
extern int line_number;
void yyerror(const char *s);

std::unique_ptr<BlockStmt> root;

// 辅助函数，将操作符转换为 BinaryOp
BinaryOp tokenToBinaryOp(int token) {
    switch (token) {
        case '+': return BinaryOp::ADD;
        case '-': return BinaryOp::SUB;
        case '*': return BinaryOp::MUL;
        case '/': return BinaryOp::DIV;
        case '%': return BinaryOp::MOD;
        case '<': return BinaryOp::LT;
        case '>': return BinaryOp::GT;
        case LE: return BinaryOp::LT_EQ;
        case GE: return BinaryOp::GT_EQ;
        case AND: return BinaryOp::AND_OP;
        case OR: return BinaryOp::OR_OP;
        case CONC: return BinaryOp::CONCAT;
        default: throw std::runtime_error("Unknown binary operator");
    }
}

// 辅助函数，将操作符转换为 UnaryOp
UnaryOp tokenToUnaryOp(int token) {
    switch (token) {
        case '-': return UnaryOp::NEG;
        case NOT: return UnaryOp::NOT_OP;
        case '#': return UnaryOp::LEN;
        default: throw std::runtime_error("Unknown unary operator");
    }
}
%}

%union {
    double number;
    char* string;
    Expr* expr;
    Stmt* stmt;
    std::vector<std::unique_ptr<Stmt>>* stmtList;
    std::vector<std::string>* identList;
    std::vector<std::unique_ptr<Expr>>* exprList;
}

%token <number> NUMBER
%token <string> STRING IDENTIFIER
%token LOCAL IF THEN ELSE ELSEIF WHILE DO REPEAT UNTIL FUNCTION END RETURN NIL
%token AND OR NOT NE LE GE CONC

%type <expr> expr primary_expr
%type <stmt> stmt function_decl return_stmt if_stmt while_stmt repeat_stmt
%type <stmtList> stmt_list
%type <identList> param_list
%type <exprList> expr_list arg_list

%left OR
%left AND
%left '<' LE '>' GE '=' NE
%left CONC
%left '+' '-'
%left '*' '/' '%'
%right NOT
%right '^'

%%

program     : stmt_list
    {
        std::vector<std::unique_ptr<Stmt>> stmts;
        for (auto& stmt : *$1) {
            stmts.push_back(std::move(stmt));
        }
        root = std::make_unique<BlockStmt>(std::move(stmts));
        delete $1;
    }
    ;

stmt_list   : stmt
    {
        $$ = new std::vector<std::unique_ptr<Stmt>>();
        if ($1) {
            $$->push_back(std::unique_ptr<Stmt>($1));
        }
    }
    | stmt_list stmt
    {
        if ($2) {
            $1->push_back(std::unique_ptr<Stmt>($2));
        }
        $$ = $1;
    }
    ;

stmt        : ';'                         { $$ = nullptr; }
            | function_decl               { $$ = $1; }
            | return_stmt                 { $$ = $1; }
            | if_stmt                     { $$ = $1; }
            | while_stmt                  { $$ = $1; }
            | repeat_stmt                 { $$ = $1; }
            | expr                        { $$ = new ExprStmt(std::unique_ptr<Expr>($1)); }
            ;

function_decl: FUNCTION IDENTIFIER '(' param_list ')' stmt_list END
    {
        std::vector<std::unique_ptr<Stmt>> body;
        for (auto& stmt : *$6) {
            body.push_back(std::move(stmt));
        }
        $$ = new FunctionDecl($2, *$4, std::move(body));
        delete $4;
        delete $6;
    }
    ;

param_list  : /* empty */                { $$ = new std::vector<std::string>(); }
            | IDENTIFIER                  { $$ = new std::vector<std::string>();
                                          $$->push_back($1); }
            | param_list ',' IDENTIFIER   { $1->push_back($3); $$ = $1; }
            ;

return_stmt : RETURN expr_list
    {
        std::vector<std::unique_ptr<Expr>> exprs;
        for (auto& expr : *$2) {
            exprs.push_back(std::move(expr));
        }
        $$ = new ReturnStmt(std::move(exprs));
        delete $2;
    }
    | RETURN
    {
        $$ = new ReturnStmt(std::vector<std::unique_ptr<Expr>>());
    }
    ;

if_stmt     : IF expr THEN stmt_list ELSE stmt_list END
    {
        std::vector<std::unique_ptr<Stmt>> thenStmts;
        std::vector<std::unique_ptr<Stmt>> elseStmts;
        
        for (auto& stmt : *$4) {
            thenStmts.push_back(std::move(stmt));
        }
        for (auto& stmt : *$6) {
            elseStmts.push_back(std::move(stmt));
        }
        
        $$ = new IfStmt(
            std::unique_ptr<Expr>($2),
            std::make_unique<BlockStmt>(std::move(thenStmts)),
            std::make_unique<BlockStmt>(std::move(elseStmts))
        );
        delete $4;
        delete $6;
    }
    ;

while_stmt  : WHILE expr DO stmt_list END
    {
        std::vector<std::unique_ptr<Stmt>> body;
        for (auto& stmt : *$4) {
            body.push_back(std::move(stmt));
        }
        $$ = new WhileStmt(
            std::unique_ptr<Expr>($2),
            std::make_unique<BlockStmt>(std::move(body))
        );
        delete $4;
    }
    ;

repeat_stmt : REPEAT stmt_list UNTIL expr
    {
        std::vector<std::unique_ptr<Stmt>> body;
        for (auto& stmt : *$2) {
            body.push_back(std::move(stmt));
        }
        $$ = new RepeatStmt(
            std::unique_ptr<Expr>($4),
            std::make_unique<BlockStmt>(std::move(body))
        );
        delete $2;
    }
    ;

expr_list   : expr
    {
        $$ = new std::vector<std::unique_ptr<Expr>>();
        $$->push_back(std::unique_ptr<Expr>($1));
    }
    | expr_list ',' expr
    {
        $1->push_back(std::unique_ptr<Expr>($3));
        $$ = $1;
    }
    ;

expr        : primary_expr               { $$ = $1; }
            | expr '+' expr              { $$ = new BinaryExpr(BinaryOp::ADD,
                                                             std::unique_ptr<Expr>($1),
                                                             std::unique_ptr<Expr>($3)); }
            | expr '-' expr              { $$ = new BinaryExpr(BinaryOp::SUB,
                                                             std::unique_ptr<Expr>($1),
                                                             std::unique_ptr<Expr>($3)); }
            | expr '*' expr              { $$ = new BinaryExpr(BinaryOp::MUL,
                                                             std::unique_ptr<Expr>($1),
                                                             std::unique_ptr<Expr>($3)); }
            | expr '/' expr              { $$ = new BinaryExpr(BinaryOp::DIV,
                                                             std::unique_ptr<Expr>($1),
                                                             std::unique_ptr<Expr>($3)); }
            | '-' expr %prec NOT         { $$ = new UnaryExpr(UnaryOp::NEG,
                                                             std::unique_ptr<Expr>($2)); }
            | NOT expr
            {
                (yyval.expr) = new UnaryExpr(UnaryOp::NOT_OP, std::unique_ptr<Expr>((yyvsp[(2) - (2)].expr)));
            }
            ;

primary_expr: NUMBER                     { $$ = new NumberExpr($1); }
            | STRING                     { $$ = new StringExpr($1); }
            | NIL                        { $$ = new NilExpr(); }
            | IDENTIFIER                 { $$ = new VarExpr($1); }
            | IDENTIFIER '(' arg_list ')'
            {
                std::vector<std::unique_ptr<Expr>> args;
                for (auto& arg : *$3) {
                    args.push_back(std::move(arg));
                }
                $$ = new CallExpr($1, std::move(args));
                delete $3;
            }
            | '(' expr ')'               { $$ = $2; }
            ;

arg_list    : /* empty */               { $$ = new std::vector<std::unique_ptr<Expr>>(); }
            | expr_list                  { $$ = $1; }
            ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", line_number, s);
} 