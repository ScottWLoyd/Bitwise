typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Decl Decl;
typedef struct Typespec Typespec;

typedef struct Note {
    SrcPos pos;
    const char* name;
} Note;

typedef struct NoteList {
    Note* notes;
    size_t num_notes;
} NoteList;

struct Type;

typedef struct StmtList {
    SrcPos pos;
    Stmt** stmts;
    size_t num_stmts;
} StmtList;

typedef enum TypespecKind {
    TYPESPEC_NONE,
    TYPESPEC_NAME,
    TYPESPEC_FUNC,
    TYPESPEC_ARRAY,
    TYPESPEC_PTR,
    TYPESPEC_CONST,
} TypespecKind;

typedef struct Typespec {
    TypespecKind kind;
    SrcPos pos;
    struct Type* type;
    Typespec* base;
    union {
        const char* name;
        struct {
            Typespec** args;
            size_t num_args;
            Typespec* ret;
            bool has_varargs;
        } func;
        struct {
            Typespec* elem;
            Expr* size;
        } array;
        struct {
            Typespec* elem;
        } ptr;
        Expr* num_elems;
    };
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

typedef struct FuncParam {
    SrcPos pos;
    const char* name;
    Typespec* type;
} FuncParam;

typedef struct AggregateItem {
    SrcPos pos;
    const char** names;
    size_t num_names;
    Typespec* type;
} AggregateItem;

typedef struct EnumItem {
    SrcPos pos;
    const char* name;
    Expr* init;
} EnumItem;

typedef struct Decl {
    DeclKind kind;
    SrcPos pos;
    const char* name;
    struct Sym* sym;
	NoteList notes;
    union {
        struct {
            EnumItem* items;
            size_t num_items;
        } enum_decl;
        struct {
            AggregateItem* items;
            size_t num_items;
        } aggregate;
        struct {
            FuncParam* params;
            size_t num_params;
            Typespec* ret_type;
            bool has_varargs;
            StmtList block;
        } func;
        struct {
            Typespec* type;
        } typedef_decl;
        struct {
            Typespec* type;
            Expr* expr;
        } var;
        struct  {
            Expr* expr;
        } const_decl;
    };
} Decl;

typedef struct DeclSet {
    Decl** decls;
    size_t num_decls;
} DeclSet;

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
	EXPR_SIZEOF_TYPE,
	EXPR_SIZEOF_EXPR,
} ExprKind;

typedef enum CompundFieldKind {
    FIELD_DEFAULT,
    FIELD_NAME,
    FIELD_INDEX,
} CompoundFieldKind;

typedef struct CompoundField {
    CompoundFieldKind kind;
    SrcPos pos;
    Expr* init;
    union {
        const char* name;
        Expr* index;
    };
} CompoundField;

typedef struct Expr {
    ExprKind kind;
    SrcPos pos;
    struct Type* type;
    union {
        struct {
            unsigned long long val;
            TokenMod mod;
            TokenSuffix suffix;
        } int_lit;
        struct {
            double val;
            TokenSuffix suffix;
        } float_lit;
        struct {
            const char* val;
            TokenMod mod;
        } str_lit;
        const char* name;
		Expr* sizeof_expr;
		Typespec* sizeof_type;
        struct {
            Typespec* type;
            CompoundField* fields;
            size_t num_fields;
        } compound;
        struct {
            Typespec* type;
            Expr* expr;
        } cast;
        struct {
            TokenKind op;
            Expr* expr;
        } unary;
        struct {
            TokenKind op;
            Expr* left;
            Expr* right;
        } binary;
        struct {
            Expr* cond;
            Expr* if_true;
            Expr* if_false;
        } ternary;
        struct {
            Expr* expr;
            Expr** args;
            size_t num_args;
        } call;
        struct {
            Expr* expr;
            Expr* index;
        } index;
        struct {
            Expr* expr;
            const char* name;
        } field;
    };
} Expr;

typedef enum StmtKind {
    STMT_NONE,
	STMT_DECL,
    STMT_RETURN,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_DO_WHILE,
    STMT_SWITCH,
    STMT_ASSIGN,
    STMT_INIT,
    STMT_EXPR,
} StmtKind;


typedef struct ElseIf {
    Expr* cond;
    StmtList block;
} ElseIf;

typedef struct SwitchCase {
    Expr** exprs;
    size_t num_exprs;
    bool is_default;
    StmtList block;
} SwitchCase;

typedef struct Stmt {
    StmtKind kind;
    SrcPos pos;
    union {
        Expr* expr;
		Decl* decl;
        struct {
            Expr* cond;
            StmtList then_block;
            ElseIf* elseifs;
            size_t num_elseifs;
            StmtList else_block;
        } if_stmt;
        struct {
            Expr* cond;
            StmtList block;
        } while_stmt;
        struct {
            Stmt* init;
            Expr* cond;
            Stmt* next;
            StmtList block;
        } for_stmt;
        struct {
            Expr* expr;
            SwitchCase* cases;
            size_t num_cases;
        } switch_stmt;
        StmtList block;
        struct {
            TokenKind op;
            Expr* left;
            Expr* right;
        } assign;
        struct {
            const char* name;
            Typespec* type;
            Expr* expr;
        } init;
    };
} Stmt;
