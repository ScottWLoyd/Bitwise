
typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Decl Decl;
typedef struct Typespec Typespec;

typedef enum TypespecKind {
    TYPESPEC_NONE,
    TYPESPEC_NAME,
    TYPESPEC_PAREN,
    TYPESPEC_FUNC,
    TYPESPEC_ARRAY,
    TYPESPEC_POINTER,
} TypespecKind;

typedef struct Typespec {
    TypespecKind kind;
} Typespec;

typedef enum DeclKind {
    DECL_NONE,
    DECL_ENUM,
    DECL_STRUCT,
    DECL_UNION,
    DECL_VAR,
    DECL_CONST,
    DECL_TYPEDEF,
    DECL_FUNC,
} DeclKind;

typedef struct EnumItem {
    const char* name;
    Typespec* type;
} EnumItem;

typedef struct AggregateItem {
    BUF(const char*) names;
    Typespec* type;
} AggregateItem;

typedef struct FuncParam {
    const char* name;
    Typespec* type;
} FuncParam;

typedef struct FuncDecl {
    BUF(FuncParam) params;  
    Typespec* ret_type;
} FuncDecl;

typedef struct Decl {
    DeclKind kind;
    const char* name;
    union {
        BUF(EnumItem) enum_items; 
        BUF(AggregateItem) aggregate_items; 
        struct {
            Typespec* type;
            Expr* expr;
        };
        FuncDecl func_decl;
    };
} Decl;

typedef enum ExprKind {
    EXPR_NONE,
    EXPR_INT,
    EXPR_FLOAT,
    EXPR_STR,
    EXPR_NAME,
    EXPR_CAST,
    EXPR_CALL,
    EXPR_INDEX,
    EXPR_FIELD,
    EXPR_COMPOUND,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_TERNARY,
} ExprKind;

typedef struct Expr {
    ExprKind kind;
    union {
        // Literals
        uint64_t int_val;
        double float_val;
        const char* str_val;
        const char* name;
        // Compound literals
        struct {
            Typespec* compound_type;
            BUF(Expr*) compound_args; 
        };
        // Casts
        struct {
            Typespec* cast_type;
            Expr* cast_expr;
        };
        struct {
            // Unary 
            Expr* operand;
            union {
                BUF(Expr*) args; 
                Expr* index;
                const char* field;
            };
        };
        struct {
            // Binary
            Expr* left;
            Expr* right;
        };
        struct {
            // Ternary
            Expr* cond;
            Expr* then_expr;
            Expr* else_expr;
        };
    };
} Expr;

typedef enum StmtKind {
    STMT_NONE,
    STMT_RETURN,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_DO,
    STMT_SWITCH,
    STMT_ASSIGN,
    STMT_AUTO_ASSIGN,
    STMT_EXPR,
} StmtKind;

typedef struct StmtBlock {
    BUF(Stmt*) stmts;
} StmtBlock;

typedef struct ElseIf {
    Expr* cond;
    StmtBlock* block;
} ElseIf;

typedef struct Stmt {
    StmtKind kind;
    Expr* expr;
    StmtBlock block;
    union {
        struct {
            BUF(ElseIf) elseifs;
            StmtBlock else_block;            
        };
        struct {
            const char* name;
        };
        struct {
            Expr* rhs;
        };
    };
} Stmt;