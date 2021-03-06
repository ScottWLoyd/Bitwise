void print_expr(Expr* expr);
void print_stmt(Stmt* stmt);
void print_decl(Decl* decl);

int indent;

char* print_buf;
bool use_print_buf;

#define printf(...) (use_print_buf ? (void)buf_printf(print_buf, __VA_ARGS__) : (void)printf(__VA_ARGS__))

void flush_print_buf(FILE* file)
{
	if (print_buf)
	{
		if (file)
		{
			fputs(print_buf, file);
		}
		buf_clear(print_buf);
	}
}

void print_newline(void)
{
    printf("\n%.*s", 2 * indent, "                                                                      ");
}

void print_typespec(Typespec* type)
{
    Typespec* t = type;
    switch (t->kind)
    {
        case TYPESPEC_NAME:
            printf("%s", t->name);
            break;
        case TYPESPEC_FUNC:
            printf("(func (");
            for (Typespec** it = t->func.args; it != t->func.args + t->func.num_args; it++)
            {
                printf(" ");
                print_typespec(*it);
            }
            printf(" ) ");
            print_typespec(t->func.ret);
            if (t->func.ret)
            {
                print_typespec(t->func.ret);
            }
            else
            {
                printf("void");
            }
            printf(")");
            break;
        case TYPESPEC_ARRAY:
            printf("(array ");
            print_typespec(t->base);
            printf(" ");
            print_expr(t->num_elems);
            printf(")");
            break;
        case TYPESPEC_PTR:
            printf("(ptr ");
            print_typespec(t->base);
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
            printf("%llu", expr->int_lit.val);
            break;
        case EXPR_FLOAT:
            printf("%f", expr->float_lit.val);
            break;
        case EXPR_STR:
            printf("\"%s\"", expr->str_lit.val);
            break;
        case EXPR_NAME:
            printf("%s", expr->name);
            break;
		case EXPR_SIZEOF_EXPR:
			printf("(sizeof-expr ");
			print_expr(expr->sizeof_expr);
			printf(")");
			break;
		case EXPR_SIZEOF_TYPE:
			printf("(sizeof-type ");
			print_typespec(expr->sizeof_type);
			printf(")");
			break;
        case EXPR_CAST:
            printf("(cast ");
            print_typespec(expr->cast.type);
            printf(" ");
            print_expr(expr->cast.expr);
            printf(")");
            break;
        case EXPR_CALL:
            printf("(");
            print_expr(expr->call.expr);
            for (Expr** it = expr->call.args; it != expr->call.args + expr->call.num_args; it++)
            {
                printf(" ");
                print_expr(*it);
            }
            printf(")");
            break;
        case EXPR_INDEX:
            printf("(index ");
            print_expr(expr->index.expr);
            printf(" ");
            print_expr(expr->index.index);
            printf(")");
            break;
        case EXPR_FIELD:
            printf("(field ");
            print_expr(expr->field.expr);
            printf(" %s)", expr->field.name);
            break;
        case EXPR_COMPOUND:
            printf("(compound ");
            if (expr->compound.type) {
                print_typespec(expr->compound.type);
            }
            else {
                printf("nil");
            }
            for (CompoundField* it = expr->compound.fields; it != expr->compound.fields + expr->compound.num_fields; it++) {
                printf(" ");
                if (it->kind == FIELD_DEFAULT)
                {
                    printf("(nil ");
                }
                else if (it->kind == FIELD_NAME)
                {
                    printf("(name %s ", it->name);
                }
                else
                {
                    assert(it->kind == FIELD_INDEX);
                    printf("(index ");
                    print_expr(it->index);
                    printf(" ");
                }
            }
            printf(")");
            break;
        case EXPR_UNARY:
            printf("(%s ", token_kind_name(expr->unary.op));
            print_expr(expr->unary.expr);
            printf(")");
            break;
        case EXPR_BINARY:
            printf("(%s ", token_kind_name(expr->binary.op));
            print_expr(expr->binary.left);
            printf(" ");
            print_expr(expr->binary.right);
            printf(")");
            break;
        case EXPR_TERNARY:
            printf("(? ");
            print_expr(expr->ternary.cond);
            printf(" ");
            print_expr(expr->ternary.if_true);
            printf(" ");
            print_expr(expr->ternary.if_false);
            printf(")");
            break;
        default:
            assert(0);
            break;
    }
}

void print_stmt_block(StmtList block)
{
    assert(block.num_stmts != 0);
    printf("(block");
    indent++;
    for (Stmt** it = block.stmts; it != block.stmts + block.num_stmts; it++) {
        print_newline();        
        print_stmt(*it);
    }
    indent--;
    printf(")");
}

void print_stmt(Stmt* stmt)
{
    Stmt* s = stmt;
    switch (s->kind)
    {
        case STMT_DECL:
            print_decl(s->decl);
            break;
        case STMT_RETURN:
            printf("(return");
			if (s->expr)
			{
				printf(" ");
				print_expr(s->expr);
			}
            printf(")");
            break;
        case STMT_BREAK:
            printf("(break)");
            break;
        case STMT_CONTINUE:
            printf("(continue)");
            break;
        case STMT_BLOCK:
            print_stmt_block(s->block);
            break;
        case STMT_IF:
            printf("(if ");
            print_expr(s->if_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->if_stmt.then_block);
            for (ElseIf* it = s->if_stmt.elseifs; it != s->if_stmt.elseifs + s->if_stmt.num_elseifs; it++) {
                print_newline();
                printf("elseif ");
                print_expr(it->cond);
                print_newline();
                print_stmt_block(it->block);
            }
            if (s->if_stmt.else_block.num_stmts != 0) {
                print_newline();
                printf("else ");
                print_newline();
                print_stmt_block(s->if_stmt.else_block);
            }
            printf(")");
            indent--;
            break;
        case STMT_WHILE:
            printf("(while ");
            print_expr(s->while_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->while_stmt.block);
            indent--;
            printf(")");
            break;
        case STMT_DO_WHILE:
            printf("(do-while ");
            print_expr(s->while_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->while_stmt.block);
            indent--;
            printf(")");
            break;
        case STMT_FOR:
            printf("(for ");
            print_stmt(s->for_stmt.init);
            print_expr(s->for_stmt.cond);
            print_stmt(s->for_stmt.next);
            indent++;
            print_newline();
            print_stmt_block(s->for_stmt.block);
            indent--;
			printf(")");
            break;
        case STMT_SWITCH:
            printf("(switch ");
            print_expr(s->switch_stmt.expr);
            indent++;
            for (SwitchCase *it = s->switch_stmt.cases; it != s->switch_stmt.cases + s->switch_stmt.num_cases; it++) {
                print_newline();
                printf("(case (");
                if (it->is_default) {
                    printf("default");

                }
                else {
                    printf("nil");

                }
                for (Expr **expr = it->exprs; expr != it->exprs + it->num_exprs; expr++) {
                    printf(" ");
                    print_expr(*expr);

                }
                printf(") ");
                indent++;
                print_newline();
                print_stmt_block(it->block);
                indent--;

            }
            indent--;
			printf(")");
            break;
        case STMT_ASSIGN:
            printf("(%s ", token_kind_names[s->assign.op]);
            print_expr(s->assign.left);
			if (s->assign.right)
			{
				printf(" ");
				print_expr(s->assign.right);
			}
            printf(")");
            break;
        case STMT_INIT:
            printf("(:= %s ", s->init.name);
            print_expr(s->init.expr);
            printf(")");
            break;
        case STMT_EXPR:
            print_expr(s->expr);
            break;
        default:
            assert(0);
            break;
    }
}

void print_aggregate_decl(Decl* decl)
{
    Decl* d = decl;
    for (AggregateItem* it = d->aggregate.items; it != d->aggregate.items + d->aggregate.num_items; it++) {
        print_newline();
        printf("(");
        for (const char** name = it->names; name != it->names + it->num_names; name++) {
            printf("%s ", *name);
        }
        print_typespec(it->type);
        printf(")");
    }
}

void print_decl(Decl* decl)
{
    Decl* d = decl;
    switch (d->kind)
    {
        case DECL_ENUM:
            printf("(enum %s", d->name);
            indent++;
            for (EnumItem* it = d->enum_decl.items; it != d->enum_decl.items + d->enum_decl.num_items; it++) {
                print_newline();
                printf("(%s ", it->name);
                if (it->init) {
                    print_expr(it->init);
                }
                else {
                    printf("nil");
                }
                printf(")");
            }
            indent--;
            printf(")");
            break;
        case DECL_STRUCT:
            printf("(struct %s", d->name);
            indent++;
            print_aggregate_decl(d);
            indent--;
            printf(")");
            break;
        case DECL_UNION:
            printf("(union %s", d->name);
            indent++;
            print_aggregate_decl(d);
            indent--;
            printf(")");
            break;
        case DECL_VAR:
            printf("(var %s ", d->name);
			if (d->var.type) {
				print_typespec(d->var.type);
			}
			else {
				printf("nil");
			}
            printf(" ");
            if (d->var.expr)
            {
                print_expr(d->var.expr);
            }
            else
            {
                printf("nil");
            }
            printf(")");
            break;
        case DECL_CONST:
            printf("(const %s ", d->name);
            print_expr(d->const_decl.expr);
            printf(")");
            break;
        case DECL_TYPEDEF:
            printf("(typedef %s ", d->name);
            print_typespec(d->typedef_decl.type);
            printf(")");
            break;
        case DECL_FUNC:
            printf("(func %s ", d->name);
            printf("(");
            for (FuncParam* it = d->func.params; it != d->func.params + d->func.num_params; it++) {
                printf(" %s ", it->name);
                print_typespec(it->type);
            }
            printf(" ) ");
			if (d->func.ret_type) {
				print_typespec(d->func.ret_type);
			}
			else {
				printf("nil");
			}
            indent++;
            print_newline();
            print_stmt_block(d->func.block);
            indent--;
            printf(")");
            break;
        default:
            assert(0);
            break;
    }
}
