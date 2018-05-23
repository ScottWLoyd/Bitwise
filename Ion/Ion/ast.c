
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

#define AST_DUP(x) ast_dup(x, num_##x * sizeof(*x))

NoteList note_list(Note* notes, size_t num_notes)
{
    return (NoteList){AST_DUP(notes), num_notes};    
}

StmtList stmt_list(SrcPos pos, Stmt** stmts, size_t num_stmts)
{
    return (StmtList) { pos, AST_DUP(stmts), num_stmts };
}

///////////////////////////////////////////////////////////////////////////////
// Typespec
//

Typespec* typespec_new(TypespecKind kind, SrcPos pos) 
{
	Typespec* type = ast_alloc(sizeof(Typespec));
	type->kind = kind;
    type->pos = pos;
	return type;
}

Typespec* typespec_name(SrcPos pos, const char* name)
{
	Typespec* type = typespec_new(TYPESPEC_NAME, pos);
	type->name = name;
	return type;
}

Typespec* typespec_ptr(SrcPos pos, Typespec* base)
{
	Typespec* type = typespec_new(TYPESPEC_PTR, pos);
	type->base = base;
	return type;
}

Typespec* typespec_const(SrcPos pos, Typespec* base)
{
    Typespec* t = typespec_new(TYPESPEC_CONST, pos);
    t->base = base;
    return t;
}

Typespec* typespec_array(SrcPos pos, Typespec* elem, Expr* size)
{
	Typespec* type = typespec_new(TYPESPEC_ARRAY, pos);
	type->base = elem;
	type->num_elems = size;
	return type;
}

Typespec* typespec_func(SrcPos pos, Typespec** args, size_t num_args, Typespec* ret, bool has_varargs)
{
	Typespec* type = typespec_new(TYPESPEC_FUNC, pos);
    type->func.args = AST_DUP(args);
    type->func.num_args = num_args;
    type->func.ret = ret;
    type->func.has_varargs = has_varargs;
	return type;
}

///////////////////////////////////////////////////////////////////////////////
// Declarations
//

DeclSet* decl_set(Decl** decls, size_t num_decls)
{
    DeclSet* declset = ast_alloc(sizeof(DeclSet));
    declset->decls = AST_DUP(decls);
    declset->num_decls = num_decls;
    return declset;
}

Decl* decl_new(DeclKind kind, SrcPos pos, const char* name)
{
    Decl* decl = ast_alloc(sizeof(Decl));
    decl->kind = kind;
    decl->pos = pos;
    decl->name = name;
    return decl;
}

Note* get_decl_note(Decl* decl, const char* name)
{
    for (size_t i = 0; i < decl->notes.num_notes; i++)
    {
        Note* note = decl->notes.notes + i;
        if (note->name == name)
        {
            return note;
        }
    }
    return NULL;
}

bool is_decl_foreign(Decl* decl)
{
    return get_decl_note(decl, foreign_name) != NULL;
}

Decl* decl_enum(SrcPos pos, const char* name, EnumItem* items, size_t num_items)
{
    Decl* decl = decl_new(DECL_ENUM, pos, name);
    decl->enum_decl.items = AST_DUP(items);
    decl->enum_decl.num_items = num_items;
    return decl;
}

Decl* decl_aggregate(SrcPos pos, DeclKind kind, const char* name, AggregateItem* items, size_t num_items)
{
    assert(kind == DECL_STRUCT || kind == DECL_UNION);
    Decl* decl = decl_new(kind, pos, name);
    decl->aggregate.items = AST_DUP(items);
    decl->aggregate.num_items = num_items;
    return decl;
}

Decl* decl_union(SrcPos pos, const char* name, AggregateItem* items, size_t num_items)
{
    Decl* decl = decl_new(DECL_UNION, pos, name);
    decl->aggregate.items = AST_DUP(items);
    decl->aggregate.num_items = num_items;
    return decl;
}

Decl* decl_struct(SrcPos pos, const char* name, AggregateItem* items, size_t num_items)
{
    Decl* decl = decl_new(DECL_STRUCT, pos, name);
    decl->aggregate.items = AST_DUP(items);
    decl->aggregate.num_items = num_items;
    return decl;
}

Decl* decl_var(SrcPos pos, const char* name, Typespec* type, Expr* expr)
{
    Decl* decl = decl_new(DECL_VAR, pos, name);
    decl->var.type = type;
    decl->var.expr = expr;
    return decl;
}

Decl* decl_func(SrcPos pos, const char* name, FuncParam* params, size_t num_params, 
                Typespec* ret_type, bool has_varargs, StmtList block)
{
    Decl* decl = decl_new(DECL_FUNC, pos, name);
    decl->func.params = AST_DUP(params);
    decl->func.num_params = num_params;
    decl->func.ret_type = ret_type;
    decl->func.has_varargs = has_varargs;
    decl->func.block = block;
    return decl;
}

Decl* decl_const(SrcPos pos, const char* name, Expr* expr)
{
    Decl* decl = decl_new(DECL_CONST, pos, name);
    decl->const_decl.expr = expr;
    return decl;
}

Decl* decl_typedef(SrcPos pos, const char* name, Typespec* type)
{
    Decl* decl = decl_new(DECL_TYPEDEF, pos, name);
    decl->typedef_decl.type = type;
    return decl;
}

///////////////////////////////////////////////////////////////////////////////
// Expressions
//

Expr* expr_new(ExprKind kind, SrcPos pos) {
	Expr* expr = ast_alloc(sizeof(Expr));    
	expr->kind = kind;
    expr->pos = pos;
	return expr;
}

Expr* expr_sizeof_type(SrcPos pos, Typespec* type)
{
    Expr* new_expr = expr_new(EXPR_SIZEOF_TYPE, pos);
    new_expr->sizeof_type = type;
    return new_expr;
}

Expr* expr_sizeof_expr(SrcPos pos, Expr* expr)
{
    Expr* new_expr = expr_new(EXPR_SIZEOF_EXPR, pos);
    new_expr->sizeof_expr = expr;
    return new_expr;
}

Expr* expr_int(SrcPos pos, unsigned long long val, TokenMod mod, TokenSuffix suffix) {
	Expr* e = expr_new(EXPR_INT, pos);
    e->int_lit.val = val;
    e->int_lit.mod = mod;
    e->int_lit.suffix = suffix;
	return e;
}

Expr* expr_float(SrcPos pos, double val, TokenSuffix suffix) {
	Expr* e = expr_new(EXPR_FLOAT, pos);
    e->float_lit.val = val;
    e->float_lit.suffix = suffix;
	return e;
}

Expr* expr_str(SrcPos pos, const char* str) {
	Expr* new_expr = expr_new(EXPR_STR, pos);
	new_expr->str_val = str;
	return new_expr;
}

Expr* expr_name(SrcPos pos, const char* name) {
	Expr* new_expr = expr_new(EXPR_NAME, pos);
	new_expr->name = name;
	return new_expr;
}

Expr* expr_compound(SrcPos pos, Typespec* type, CompoundField* fields, size_t num_fields) {
    Expr* new_expr = expr_new(EXPR_COMPOUND, pos);
    new_expr->compound.fields = AST_DUP(fields);
    new_expr->compound.num_fields = num_fields;
    new_expr->compound.type = type;
    return new_expr;
}

Expr* expr_cast(SrcPos pos, Typespec* type, Expr* expr) {
	Expr* new_expr = expr_new(EXPR_CAST, pos);
	new_expr->cast.type = type;
	new_expr->cast.expr = expr;
	return new_expr;
}

Expr* expr_call(SrcPos pos, Expr* expr, Expr** args, size_t num_args) {
    Expr* new_expr = expr_new(EXPR_CALL, pos);
    new_expr->call.expr = expr;
    new_expr->call.num_args = num_args;
    new_expr->call.args = AST_DUP(args);
    return new_expr;
}

Expr* expr_index(SrcPos pos, Expr* expr, Expr* index) {
    Expr* new_expr = expr_new(EXPR_INDEX, pos);
    new_expr->index.expr = expr;
    new_expr->index.index = index;
    return new_expr;
}

Expr* expr_field(SrcPos pos, Expr* expr, const char* name) {
    Expr* new_expr = expr_new(EXPR_FIELD, pos);
    new_expr->field.expr = expr;
    new_expr->field.name = name;
    return new_expr;
}

Expr* expr_unary(SrcPos pos, TokenKind op, Expr* expr) {
    Expr* new_expr = expr_new(EXPR_UNARY, pos);
    new_expr->unary.op = op;
    new_expr->unary.expr = expr;
    return new_expr;
}

Expr* expr_binary(SrcPos pos, TokenKind op, Expr* left, Expr* right) {
    Expr* new_expr = expr_new(EXPR_BINARY, pos);
    new_expr->binary.op = op;
    new_expr->binary.left = left;
    new_expr->binary.right = right;
    return new_expr;
}

Expr* expr_ternary(SrcPos pos, Expr* cond, Expr* if_true, Expr* if_false) {
    Expr* new_expr = expr_new(EXPR_TERNARY, pos);
    new_expr->ternary.cond = cond;
    new_expr->ternary.if_true = if_true;
    new_expr->ternary.if_false = if_false;
    return new_expr;
}

///////////////////////////////////////////////////////////////////////////////
// Expressions
// 

Stmt* stmt_new(StmtKind kind, SrcPos pos)
{
    Stmt* s = ast_alloc(sizeof(Stmt));
    s->kind = kind;
    s->pos = pos;
    return s;
}

Stmt* stmt_decl(SrcPos pos, Decl* decl)
{
	Stmt* s = stmt_new(STMT_DECL, pos);
	s->decl = decl;
	return s;
}

Stmt* stmt_return(SrcPos pos, Expr* expr)
{
    Stmt* s = stmt_new(STMT_RETURN, pos);
    s->expr = expr;
    return s;
}

Stmt* stmt_break(SrcPos pos)
{
    Stmt* s = stmt_new(STMT_BREAK, pos);
    return s;
}

Stmt* stmt_continue(SrcPos pos)
{
    Stmt* s = stmt_new(STMT_CONTINUE, pos);
    return s;
}

Stmt* stmt_block(SrcPos pos, StmtList block)
{
    Stmt* s = stmt_new(STMT_BLOCK, pos);
    s->block = block;
    return s;
}

Stmt* stmt_if(SrcPos pos, Expr* cond, StmtList then_block, ElseIf* elseifs, size_t num_elseifs, StmtList else_block)
{
    Stmt* s = stmt_new(STMT_IF, pos);
    s->if_stmt.cond = cond;
    s->if_stmt.then_block = then_block;
    s->if_stmt.elseifs = AST_DUP(elseifs);
    s->if_stmt.num_elseifs = num_elseifs;
    s->if_stmt.else_block = else_block;
    return s;
}

Stmt* stmt_while(SrcPos pos, Expr* cond, StmtList block)
{
    Stmt* s = stmt_new(STMT_WHILE, pos);
    s->while_stmt.cond = cond;
    s->while_stmt.block = block;
    return s;
}

Stmt* stmt_do_while(SrcPos pos, Expr* cond, StmtList block)
{
    Stmt* s = stmt_new(STMT_DO_WHILE, pos);
    s->while_stmt.cond = cond;
    s->while_stmt.block = block;
    return s;
}

Stmt* stmt_for(SrcPos pos, Stmt* init, Expr* cond, Stmt* next, StmtList block)
{
    Stmt* s = stmt_new(STMT_FOR, pos);
    s->for_stmt.init = init;
    s->for_stmt.cond = cond;
    s->for_stmt.next = next;
    s->for_stmt.block = block;
    return s;
}

Stmt* stmt_switch(SrcPos pos, Expr* expr, SwitchCase* cases, size_t num_cases)
{
    Stmt* s = stmt_new(STMT_SWITCH, pos);
    s->switch_stmt.expr = expr;
    s->switch_stmt.cases = AST_DUP(cases);
    s->switch_stmt.num_cases = num_cases;
    return s;
}

Stmt* stmt_assign(SrcPos pos, TokenKind op, Expr* left, Expr* right)
{
    Stmt* s = stmt_new(STMT_ASSIGN, pos);
    s->assign.op = op;
    s->assign.left = left;
    s->assign.right = right;
    return s;
}

Stmt* stmt_init(SrcPos pos, const char* name, Typespec* type, Expr* expr)
{
    Stmt* s = stmt_new(STMT_INIT, pos);
    s->init.name = name;
    s->init.type = type;
    s->init.expr = expr;
    return s;
}

Stmt* stmt_expr(SrcPos pos, Expr* expr)
{
    Stmt* s = stmt_new(STMT_EXPR, pos);
    s->expr = expr;
    return s;
}

#undef AST_DUP