
Arena ast_arena;

void* ast_alloc(size_t size) {
    assert(size != 0);
    void* ptr = arena_alloc(&ast_arena, size);
    memset(ptr, 0, size);
    return ptr;
}

void* ast_dup(const void* src, size_t size)
{
    if (size == 0)
    {
        return NULL;
    }
    void* ptr = arena_alloc(&ast_arena, size);
    memcpy(ptr, src, size);
    return ptr;
}

///////////////////////////////////////////////////////////////////////////////
// Typespec
//

Typespec* typespec_new(TypespecKind kind) 
{
	Typespec* type = ast_alloc(sizeof(Typespec));
	type->kind = kind;
	return type;
}

Typespec* typespec_name(const char* name)
{
	Typespec* type = typespec_new(TYPESPEC_NAME);
	type->name = name;
	return type;
}

Typespec* typespec_ptr(Typespec* elem)
{
	Typespec* type = typespec_new(TYPESPEC_PTR);
	type->ptr.elem = elem;
	return type;
}

Typespec* typespec_array(Typespec* elem, Expr* size)
{
	Typespec* type = typespec_new(TYPESPEC_ARRAY);
	type->array.elem = elem;
	type->array.size = size;
	return type;
}

Typespec* typespec_func(Typespec** args, size_t num_args, Typespec* ret)
{
	Typespec* type = typespec_new(TYPESPEC_FUNC);
    type->func.args = args;
    type->func.num_args = num_args;
    type->func.ret = ret;
	return type;
}

///////////////////////////////////////////////////////////////////////////////
// Declarations
//

Decl* decl_new(DeclKind kind, const char* name)
{
    Decl* decl = ast_alloc(sizeof(Decl));
    decl->kind = kind;
    decl->name = name;
    return decl;
}

Decl* decl_enum(const char* name, EnumItem* items, size_t num_items)
{
    Decl* decl = decl_new(DECL_ENUM, name);
    decl->enum_decl.items = items;
    decl->enum_decl.num_items = num_items;
    return decl;
}

Decl* decl_aggregate(DeclKind kind, const char* name, AggregateItem* items, size_t num_items)
{
    assert(kind == DECL_STRUCT || kind == DECL_UNION);
    Decl* decl = decl_new(kind, name);
    decl->aggregate.items = items;
    decl->aggregate.num_items = num_items;
    return decl;
}

Decl* decl_union(const char* name, AggregateItem* items, size_t num_items)
{
    Decl* decl = decl_new(DECL_UNION, name);
    decl->aggregate.items = items;
    decl->aggregate.num_items = num_items;
    return decl;
}

Decl* decl_struct(const char* name, AggregateItem* items, size_t num_items)
{
    Decl* decl = decl_new(DECL_STRUCT, name);
    decl->aggregate.items = items;
    decl->aggregate.num_items = num_items;
    return decl;
}

Decl* decl_var(const char* name, Typespec* type, Expr* expr)
{
    Decl* decl = decl_new(DECL_VAR, name);
    decl->var.type = type;
    decl->var.expr = expr;
    return decl;
}

Decl* decl_func(const char* name, FuncParam* params, size_t num_params, Typespec* ret_type, StmtBlock block)
{
    Decl* decl = decl_new(DECL_FUNC, name);
    decl->func.params = params;
    decl->func.num_params = num_params;
    decl->func.ret_type = ret_type;
    decl->func.block = block;
    return decl;
}

Decl* decl_const(const char* name, Expr* expr)
{
    Decl* decl = decl_new(DECL_CONST, name);
    decl->const_decl.expr = expr;
    return decl;
}

Decl* decl_typedef(const char* name, Typespec* type)
{
    Decl* decl = decl_new(DECL_TYPEDEF, name);
    decl->typedef_decl.type = type;
    return decl;
}

///////////////////////////////////////////////////////////////////////////////
// Expressions
//

Expr* expr_new(ExprKind kind) {
	Expr* expr = ast_alloc(sizeof(Expr));
	expr->kind = kind;
	return expr;
}

Expr* expr_int(uint64_t int_val) {
	Expr* new_expr = expr_new(EXPR_INT);
	new_expr->int_val = int_val;
	return new_expr;
}

Expr* expr_float(double float_val) {
	Expr* new_expr = expr_new(EXPR_FLOAT);
	new_expr->float_val = float_val;
	return new_expr;
}

Expr* expr_str(const char* str) {
	Expr* new_expr = expr_new(EXPR_STR);
	new_expr->str_val = str;
	return new_expr;
}

Expr* expr_name(const char* name) {
	Expr* new_expr = expr_new(EXPR_NAME);
	new_expr->name = name;
	return new_expr;
}

Expr* expr_compound(Typespec* type, Expr** args, size_t num_args) {
    Expr* new_expr = expr_new(EXPR_COMPOUND);
    new_expr->compound.args = args;
    new_expr->compound.num_args = num_args;
    new_expr->compound.type = type;
    return new_expr;
}

Expr* expr_cast(Typespec* type, Expr* expr) {
	Expr* new_expr = expr_new(EXPR_CAST);
	new_expr->cast.type = type;
	new_expr->cast.expr = expr;
	return new_expr;
}

Expr* expr_unary(TokenKind op, Expr* expr) {
	Expr* new_expr = expr_new(EXPR_UNARY);
	new_expr->unary.op = op;
	new_expr->unary.expr = expr;
	return new_expr;
}

Expr* expr_binary(TokenKind op, Expr* left, Expr* right) {
	Expr* new_expr = expr_new(EXPR_BINARY);
	new_expr->binary.op = op;
	new_expr->binary.left = left;
	new_expr->binary.right = right;
	return new_expr;
}

Expr* expr_ternary(Expr* cond, Expr* if_true, Expr* if_false) {
	Expr* new_expr = expr_new(EXPR_TERNARY);
	new_expr->ternary.cond = cond;
	new_expr->ternary.if_true = if_true;
	new_expr->ternary.if_false = if_false;
	return new_expr;
}

Expr* expr_call(Expr* expr, Expr** args, size_t num_args) {
    Expr* new_expr = expr_new(EXPR_CALL);
    new_expr->call.expr = expr;
    new_expr->call.num_args = num_args;
    new_expr->call.args = args;
    return new_expr;
}

Expr* expr_index(Expr* expr, Expr* index) {
    Expr* new_expr = expr_new(EXPR_INDEX);
    new_expr->index.expr = expr;
    new_expr->index.index = index;
    return new_expr;
}

Expr* expr_field(Expr* expr, const char* name) {
    Expr* new_expr = expr_new(EXPR_FIELD);
    new_expr->field.expr = expr;
    new_expr->field.name = name;
    return new_expr;
}

///////////////////////////////////////////////////////////////////////////////
// Expressions
// 

Stmt* stmt_new(StmtKind kind)
{
    Stmt* s = ast_alloc(sizeof(Stmt));
    s->kind = kind;
    return s;
}

Stmt* stmt_return(Expr* expr)
{
    Stmt* s = stmt_new(STMT_RETURN);
    s->return_stmt.expr = expr;
    return s;
}

Stmt* stmt_break()
{
    Stmt* s = stmt_new(STMT_BREAK);
    return s;
}

Stmt* stmt_continue()
{
    Stmt* s = stmt_new(STMT_CONTINUE);
    return s;
}

Stmt* stmt_block(StmtBlock block)
{
    Stmt* s = stmt_new(STMT_BLOCK);
    s->block = block;
    return s;
}

Stmt* stmt_if(Expr* cond, StmtBlock then_block, ElseIf* elseifs, size_t num_elseifs, StmtBlock else_block)
{
    Stmt* s = stmt_new(STMT_IF);
    s->if_stmt.cond = cond;
    s->if_stmt.then_block = then_block;
    s->if_stmt.elseifs = elseifs;
    s->if_stmt.num_elseifs = num_elseifs;
    s->if_stmt.else_block = else_block;
    return s;
}

Stmt* stmt_while(Expr* cond, StmtBlock block)
{
    Stmt* s = stmt_new(STMT_WHILE);
    s->while_stmt.cond = cond;
    s->while_stmt.block = block;
    return s;
}

Stmt* stmt_do_while(Expr* cond, StmtBlock block)
{
    Stmt* s = stmt_new(STMT_DO_WHILE);
    s->while_stmt.cond = cond;
    s->while_stmt.block = block;
    return s;
}

Stmt* stmt_for(Stmt* init, Expr* cond, Stmt* next, StmtBlock block)
{
    Stmt* s = stmt_new(STMT_FOR);
    s->for_stmt.init = init;
    s->for_stmt.cond = cond;
    s->for_stmt.next = next;
    s->for_stmt.block = block;
    return s;
}

Stmt* stmt_switch(Expr* expr, SwitchCase* cases, size_t num_cases)
{
    Stmt* s = stmt_new(STMT_SWITCH);
    s->switch_stmt.expr = expr;
    s->switch_stmt.cases = cases;
    s->switch_stmt.num_cases = num_cases;
    return s;
}

Stmt* stmt_assign(TokenKind op, Expr* left, Expr* right)
{
    Stmt* s = stmt_new(STMT_ASSIGN);
    s->assign.op = op;
    s->assign.left = left;
    s->assign.right = right;
    return s;
}

Stmt* stmt_init(const char* name, Expr* expr)
{
    Stmt* s = stmt_new(STMT_INIT);
    s->init.name = name;
    s->init.expr = expr;
    return s;
}

Stmt* stmt_expr(Expr* expr)
{
    Stmt* s = stmt_new(STMT_EXPR);
    s->expr = expr;
    return s;
}