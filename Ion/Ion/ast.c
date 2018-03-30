
typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Decl Decl;
typedef struct Typespec Typespec;

typedef enum TypespecKind {
    TYPESPEC_NONE,
    TYPESPEC_NAME,
    TYPESPEC_FUNC,
    TYPESPEC_ARRAY,
    TYPESPEC_POINTER,
} TypespecKind;

typedef struct FuncTypespec {
	BUF(Typespec*) args;
	Typespec* ret;
} FuncTypespec;

typedef struct Typespec {
    TypespecKind kind;
	struct {
		const char* name;
		FuncTypespec func;
		struct {
			Typespec* base;
			Expr* size;
		};
	};
} Typespec;

Typespec* typespec_alloc(TypespecKind kind) 
{
	Typespec* type = xcalloc(1, sizeof(Typespec));
	type->kind = kind;
	return type;
}

Typespec* typespec_name(const char* name)
{
	Typespec* type = typespec_alloc(TYPESPEC_NAME);
	type->name = name;
	return type;
}

Typespec* typespec_pointer(Typespec* base)
{
	Typespec* type = typespec_alloc(TYPESPEC_POINTER);
	type->base = base;
	return type;
}

Typespec* typespec_array(Typespec* base, int size)
{
	Typespec* type = typespec_alloc(TYPESPEC_POINTER);
	type->base = base;
	type->size = size;
	return type;
}

Typespec* typespec_func(FuncTypespec func)
{
	Typespec* type = typespec_alloc(TYPESPEC_FUNC);
	type->func = func;
	return type;
}

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
    const char** names;
	size_t num_names;
    Typespec* type;
} AggregateItem;

typedef struct FuncParam {
    const char* name;
    Typespec* type;
} FuncParam;

typedef struct FuncDecl {
    FuncParam* params;  
	size_t num_params;
    Typespec* ret_type;
} FuncDecl;

typedef struct Decl {
    DeclKind kind;
    const char* name;
    union {
		struct {
			size_t num_enum_items;
			EnumItem* enum_items;
		};
		struct {
			size_t num_aggregate_items;
			AggregateItem* aggregate_items;
		};
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
	TokenKind op;
    union {
        // Literals
        uint64_t int_val;
        double float_val;
        const char* str_val;
        const char* name;
        // Compound literals
        struct {
            Typespec* compound_type;
			size_t num_compound_args;
            Expr** compound_args; 
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
				struct {
					size_t num_args;
					Expr** args;
				};
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

Expr* expr_alloc(ExprKind kind) {
	Expr* expr = (Expr*)xcalloc(1, sizeof(Expr));
	expr->kind = kind;
	return expr;
}

Expr* expr_int(uint64_t int_val) {
	Expr* new_expr = expr_alloc(EXPR_INT);
	new_expr->int_val = int_val;
	return new_expr;
}

Expr* expr_float(double float_val) {
	Expr* new_expr = expr_alloc(EXPR_FLOAT);
	new_expr->float_val = float_val;
	return new_expr;
}

Expr* expr_str(const char* str) {
	Expr* new_expr = expr_alloc(EXPR_STR);
	new_expr->str_val = str;
	return new_expr;
}

Expr* expr_name(const char* name) {
	Expr* new_expr = expr_alloc(EXPR_NAME);
	new_expr->name = name;
	return new_expr;
}

Expr* expr_cast(Typespec* type, Expr* expr) {
	Expr* new_expr = expr_alloc(EXPR_CAST);
	new_expr->cast_type = type;
	new_expr->cast_expr = expr;
	return new_expr;
}

Expr* expr_call(Expr* operand, Expr** args) {
	Expr* new_expr = expr_alloc(EXPR_CALL);
	new_expr->operand = operand;
	new_expr->args = args;
	return new_expr;
}

Expr* expr_index(Expr* operand, Expr* index) {
	Expr* new_expr = expr_alloc(EXPR_INDEX);
	new_expr->operand = operand;
	new_expr->index = index;
	return new_expr;
}

Expr* expr_field(Expr* operand, const char* field) {
	Expr* new_expr = expr_alloc(EXPR_FIELD);
	new_expr->operand = operand;
	new_expr->field = field;
	return new_expr;
}

Expr* expr_unary(TokenKind op, Expr* expr) {
	Expr* new_expr = expr_alloc(EXPR_UNARY);
	new_expr->op = op;
	new_expr->operand = expr;
	return new_expr;
}

Expr* expr_binary(TokenKind op, Expr* left, Expr* right) {
	Expr* new_expr = expr_alloc(EXPR_BINARY);
	new_expr->op = op;
	new_expr->left = left;
	new_expr->right = right;
	return new_expr;
}

Expr* expr_ternary(Expr* cond, Expr* then_expr, Expr* else_expr) {
	Expr* new_expr = expr_alloc(EXPR_TERNARY);
	new_expr->cond = cond;
	new_expr->then_expr = then_expr;
	new_expr->else_expr = else_expr;
	return new_expr;
}

void print_expr(Expr* expr);

void print_type(Typespec* type)
{
	switch (type->kind)
	{
		case TYPESPEC_NAME:
			printf("%s", type->name);
			break;
		case TYPESPEC_FUNC:
			printf("(func ");
			for (Typespec** it = type->func.args; it != buf_end(type->func.args); it++)
			{
				printf(" ");
				print_type(*it);
			}
			printf(")");
			print_type(type->func.ret);
			printf(")");
			break;
		case TYPESPEC_ARRAY:
			printf("(arr ");
			print_type(type->base);
			printf(" ");
			print_expr(type->size);
			printf(")");
			break;
		case TYPESPEC_POINTER:
			printf("(ptr ");
			print_type(type->base);
			printf(")");
			break;
		default:
			assert(0);
			break;
	}
}

void print_expr(Expr* expr)
{
	switch (expr->kind)
	{
		case EXPR_NONE: assert(0); break;
		case EXPR_INT: 
			printf("%" PRIu64, expr->int_val);
			break;
		case EXPR_FLOAT:
			printf("%f", expr->float_val);
			break;
		case EXPR_STR:
			printf("\"%s\"", expr->str_val);
			break;
		case EXPR_NAME:
			printf("%s", expr->name);
			break;
		case EXPR_CAST:
			printf("(cast ");
			print_type(expr->cast_type);
			printf(" ");
			print_expr(expr->cast_expr);
			printf(")");
			break;
		case EXPR_CALL:
			printf("(");
			print_expr(expr->operand);
			for (Expr** it = expr->args; it != buf_end(expr->args); it++)
			{
				printf(" ");
				print_expr(*it);
			}
			printf(")");
			break;
		case EXPR_INDEX:
			printf("(index ");
			print_expr(expr->operand);
			printf(" ");
			print_expr(expr->index);
			printf(")");
			break;
		case EXPR_FIELD:
			printf("(field ");
			print_expr(expr->operand);
			printf(" %s)", expr->field);
			break;
		case EXPR_COMPOUND:
			printf("(compound ...)");
			break;
		case EXPR_UNARY:
			printf("(%c ", expr->op);
			print_expr(expr->operand);
			printf(")");
			break;
		case EXPR_BINARY:
			printf("(%c ", expr->op);
			print_expr(expr->left);
			printf(" ");
			print_expr(expr->right);
			printf(")");
			break;
		case EXPR_TERNARY:
			printf("(if ");
			print_expr(expr->cond); 
			printf(" ");
			print_expr(expr->then_expr);
			printf(" ");
			print_expr(expr->else_expr);
			printf(")");
			break;
		default:
			assert(0);
			break;
	}
}
void print_expr_line(Expr* expr)
{
	print_expr(expr);
	printf("\n");
}

void expr_test() 
{
	Expr** fact_args = NULL;
	buf_push(fact_args, expr_int(32));
	Expr* exprs[] = {
		expr_binary('+', expr_int(1), expr_int(2)),
		expr_unary('-', expr_float(3.14)),
		expr_ternary(expr_name("flag"), expr_str("true"), expr_str("false")),
		expr_field(expr_name("person"), "name"),
		expr_call(expr_name("fact"), fact_args),
		expr_index(expr_field(expr_name("person"), "siblings"), expr_int(3)),
		expr_cast(typespec_pointer(typespec_name("int")), expr_name("void_ptr")),
	};
	for (Expr** it = exprs; it != exprs + sizeof(exprs)/sizeof(exprs[0]); it++)
	{
		print_expr_line(*it);
	}
}

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
    StmtBlock block;
} ElseIf;

typedef struct Case {
	BUF(Expr*) exprs;
	StmtBlock block;
} Case;

typedef struct Stmt {
    StmtKind kind;
    Expr* expr;
    StmtBlock block;
    union {
		// If
        struct {
            BUF(ElseIf) elseifs;
            StmtBlock else_block;
        };
		// For
		struct {
			StmtBlock for_init;
			StmtBlock for_next;
		};
		// Switch
		struct {
			BUF(Case) cases;
		};
		// Auto-assign
        struct {
            const char* name;
        };
		// Assignment operators
        struct {
            Expr* rhs;
        };
    };
} Stmt;


void ast_test()
{
	expr_test();
}