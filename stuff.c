#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* tokenization */

enum name
{
    id,
    num,
    newline,
    EOI
};

struct token
{
    enum name name;
    char *val;
};

struct token getNextToken(char **str)
{
    struct token t;
    t.val = calloc(1, 1);
    int vsize = 1;
    
    int state = 0;
    for(str; ; (*str)++)
    {
        switch(state)
        {
            /* error state */
            case -1:
                exit(-1);
            break;
            
            case 0:
                state = -1;
                if(isspace(**str) && **str != '\n') state = 0;
                if(isdigit(**str)) state = 1;
                if(isalpha(**str)) state = 2;
                if(**str == '\n') state = 3;
                if(**str == '\0') state = 4;
            break;
            
            case 1:
                state = -1;
                if(isdigit(**str)) state = 1;
                if(isspace(**str) || **str == '\n' || **str == '\0')
                {
                    t.name = num;
                    return t;
                }
            break;
            
            case 2:
                state = -1;
                if(isdigit(**str) || isalpha(**str)) state = 2;
                if(isspace(**str) || **str == '\n' || **str == '\0')
                {
                    t.name = id;
                    return t;
                }
            break;
            
            case 3:
                t.name = newline;
                return t;
            break;
            
            case 4:
                t.name = EOI;
                return t;
            break;
        }
        
        /* error message */
        if(state == -1) printf("Error: expected character: %c\n", **str);
        
        /* store token value */
        if((!isspace(**str) || **str == '\n') && **str != '\0')
        {
            char temp[2];
            temp[0] = **str;
            temp[1] = '\0';
            
            t.val = realloc(t.val, vsize+1);
            strcat(t.val, temp);
            vsize++;
        }
    }
}

void expectVal(char **str, struct token *tk, char *test)
{
    if(strcmp(tk->val, test) != 0)
    {
        printf("ERROR: expected token value: %s\n", test);
        exit(-1);
    }
    
    *tk = getNextToken(str);
}

void expectName(char **str, struct token *tk, enum name test)
{
    if(tk->name != test)
    {
        printf("ERROR: expected token name: %d\n", test);
        exit(-1);
    }
    
    if(tk->name != EOI) *tk = getNextToken(str);
}

/* parsing */

struct entry
{
    char *id;
    int val;
};

struct table
{
    struct entry *entry;
    int size;
};

struct stack
{
    struct table **tb;
    int top;
};

struct table * createEntry(struct table *tb, char *str, int val)
{
    int i;
    for(i = 0; i<tb->size; i++)
    {
        if(strcmp(tb->entry[i].id, str) == 0)
        {
            printf("ERROR: redeclaration of %s\n", tb->entry[i].id);
            exit(-1);
        }
    }

    tb->size++;
    tb->entry = realloc(tb->entry, tb->size * sizeof(struct entry));
    tb->entry[tb->size-1].id = str;
    tb->entry[tb->size-1].val = val;

    return tb;
}

/* push a pointer to a table to the stack */
struct stack pushTablePtr(struct stack st)
{
    st.top++;
    st.tb = realloc(st.tb, (st.top+1) * sizeof(struct table *));
    st.tb[st.top] = malloc(sizeof(struct table));

    st.tb[st.top]->entry = NULL;
    st.tb[st.top]->size = 0;

    return st;
}

/* pop a pointer to a table from the stack */
struct stack popTablePtr(struct stack st, struct table ***saveTb, int *saveTableSize)
{
    *saveTb = realloc(*saveTb, (*saveTableSize)+1 * sizeof(struct table *));
    (*saveTb)[*saveTableSize] = st.tb[st.top];
    (*saveTableSize)++;
    
    if(st.top == 0)
    {
        st.top--;
        st.tb = NULL;
        return st;
    } else if(st.top == -1)
    {
        printf("ERROR: cannot pop; nothing on the stack\n");
        exit(-1);
    }

    st.top--;
    st.tb = realloc(st.tb, (st.top+1) * sizeof(struct table *));

    return st;
}

struct entry * search(struct stack st, char *test)
{
    int i;
    for(i = st.top; i>=0; i--)
    {
        int j;
        for(j = st.tb[i]->size-1; j>=0; j--)
        {
            if(strcmp(st.tb[i]->entry[j].id, test) == 0)
            {
                return &(st.tb[i]->entry[j]);
            }
        }
    }
    
    printf("ERROR: \"%s\" has not been defined\n", test);
    exit(-1);
}

/* instruction functions */

enum instType
{
    op_add,
    op_subtract,
    op_mult,
    op_set,
    op_move,
    op_turn,
    op_jmp,
    op_jle, /* jump if less than or equal to */
    op_jge, /* jump if greater than or equal to */
    op_jne, /* jump if not equal to */
    t_label
};

struct inst
{
    enum instType type;
    int *arg1;
    int *arg2;
    int *arg3;
};

struct inst * emitInst(struct inst *instList, int *size, enum instType type, int *arg1, int *arg2, int *arg3)
{
    instList = realloc(instList, (*size+1) * sizeof(struct inst));
    
    instList[*size].type = type;
    instList[*size].arg1 = arg1;
    instList[*size].arg2 = arg2;
    instList[*size].arg3 = arg3;
    
    (*size)++;
    
    return instList;
}

/*
    block -> statement newlines block | e
    statement -> op | if | while
    oper -> add | sub | mult | move | turn | set | let
    add -> "add" (id | num) "to" id
    sub -> "subtract" (id | num) "from" id
    mult -> "multiply" id "by" (id | num)
    move -> "move" "turtle" ("forward" | "backward") (id | num) "px"
    turn -> "turn" "turtle" ("left" | "right") (id | num) "degrees"
    set -> "set" id "equal" "to" (id | num)
    let -> "let" id "equal" (id | num)
    ifst -> "if" id cond (id | num) "then" newlines block "end"
    whilest -> "while" id cond (id | num) "do" newlines block "end" "while"
    cond -> "islessthan" | "isgreaterthan" | "isequalto"
    newlines -> "\n" newlines | e
*/

void statement();
void oper();
void add();
void sub();
void mult();
void move();
void turn();
void set();
void let();
void ifst();
void whilest();
enum instType cond();
void newlines();

/* block -> statement newlines block | e */
void block(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    if(strcmp(tk->val, "add") == 0      ||
       strcmp(tk->val, "subtract") == 0 ||
       strcmp(tk->val, "multiply") == 0 ||
       strcmp(tk->val, "set") == 0      ||
       strcmp(tk->val, "move") == 0     ||
       strcmp(tk->val, "turn") == 0     ||
       strcmp(tk->val, "let") == 0      ||
       strcmp(tk->val, "if") == 0       ||
       strcmp(tk->val, "while") == 0)
    {
        statement(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
        newlines(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
        block(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    }
}

/* statement -> op | if | while */
void statement(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    if(strcmp(tk->val, "add") == 0      ||
       strcmp(tk->val, "subtract") == 0 ||
       strcmp(tk->val, "multiply") == 0 ||
       strcmp(tk->val, "move") == 0     ||
       strcmp(tk->val, "turn") == 0     ||
       strcmp(tk->val, "set") == 0      ||
       strcmp(tk->val, "let") == 0)
    {
        oper(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else if(strcmp(tk->val, "if") == 0)
    {
        ifst(str, tk, st, saveTb, saveTableSize, instList, instListSize, label); /* if statement */
    } else if(strcmp(tk->val, "while") == 0)
    {
        whilest(str, tk, st, saveTb, saveTableSize, instList, instListSize, label); /* while statement */
    } else
    {
        printf("ERROR: \"%s\" is not a valid beginning of a statement\n", tk->val);
        exit(-1);
    }
}

/* oper -> add | sub | mult | move | turn | set */
void oper(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    if(strcmp(tk->val, "add") == 0)
    {
        add(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else if(strcmp(tk->val, "subtract") == 0)
    {
        sub(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else if(strcmp(tk->val, "multiply") == 0)
    {
        mult(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else if(strcmp(tk->val, "move") == 0)
    {
        move(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else if(strcmp(tk->val, "turn") == 0)
    {
        turn(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else if(strcmp(tk->val, "set") == 0)
    {
        set(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else if(strcmp(tk->val, "let") == 0)
    {
        let(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else
    {
        printf("ERROR: malformed operation\n");
        exit(-1);
    }
}

/* add -> "add" (id | num) "to" id */
void add(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "add");
    
    int *firstArg = malloc(sizeof(int));
    if(tk->name == id)
    {   
        struct entry *tempEntry = search(*st, tk->val);
        expectName(str, tk, tk->name);
        free(firstArg);
		firstArg = &(tempEntry->val);
    } else if(tk->name == num)
    {
        *firstArg = atoi(tk->val);
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "to");
    struct entry *secondArg = search(*st, tk->val);
    *instList = emitInst(*instList, instListSize, op_add, firstArg, &(secondArg->val), NULL);
    expectName(str, tk, id);
}

/* sub -> "subtract" (id | num) "from" id */
void sub(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "subtract");
    
    int *firstArg = malloc(sizeof(int));
    if(tk->name == id)
    {   
        struct entry *tempEntry = search(*st, tk->val);
        expectName(str, tk, tk->name);
		free(firstArg);
        firstArg = &(tempEntry->val);
    } else if(tk->name == num)
    {
        *firstArg = atoi(tk->val); /* constant value temporary */
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "from");
    struct entry *secondArg = search(*st, tk->val);
    *instList = emitInst(*instList, instListSize, op_subtract, firstArg, &(secondArg->val), NULL);
    expectName(str, tk, id);
}

/* mult -> "multiply" id "by" (id | num) */
void mult(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "multiply");
    
    struct entry *firstArg = search(*st, tk->val);
    expectName(str, tk, id);
    
    expectVal(str, tk, "by");
    
    if(tk->name == id)
    {   
        struct entry *secondArg = search(*st, tk->val);
        *instList = emitInst(*instList, instListSize, op_mult, &(firstArg->val), &(secondArg->val), NULL);
        expectName(str, tk, tk->name);
    } else if(tk->name == num)
    {
        int *c = malloc(sizeof(int)); /* constant value temporary */
        *c = atoi(tk->val);
        *instList = emitInst(*instList, instListSize, op_mult, &(firstArg->val), c, NULL);
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
}

/* move -> "move" "turtle" ("forward" | "backward") (id | num) "px" */
void move(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "move");
    expectVal(str, tk, "turtle");
    
    int dir;
    if(strcmp(tk->val, "forward") == 0)
    {
        dir = 1;
        expectVal(str, tk, tk->val);
    } else if(strcmp(tk->val, "backward") == 0)
    {
        dir = -1;
        expectVal(str, tk, tk->val);
    } else
    {
        printf("ERROR: expected different direction\n");
        exit(-1);
    }
    
    if(tk->name == id)
    {
        struct entry *arg = search(*st, tk->val);
        *instList = emitInst(*instList, instListSize, op_move, &(arg->val), &dir, NULL);
        expectName(str, tk, tk->name);
    } else if(tk->name == num)
    {
        int *c = malloc(sizeof(int)); /* constant value temporary */
        *c = atoi(tk->val);
        *instList = emitInst(*instList, instListSize, op_move, c, &dir, NULL);
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "px");
}

/* turn -> "turn" "turtle" ("left" | "right") (id | num) "degrees" */
void turn(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "turn");
    expectVal(str, tk, "turtle");
    
    if(strcmp(tk->val, "left") == 0)
    {
        expectVal(str, tk, tk->val);
    } else if(strcmp(tk->val, "right") == 0)
    {
        expectVal(str, tk, tk->val);
    } else
    {
        printf("ERROR: expected different direction\n");
        exit(-1);
    }
    
    if(tk->name == id)
    {
        struct entry *arg = search(*st, tk->val);
        expectName(str, tk, tk->name);
    } else if(tk->name == num)
    {
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "degrees");
}

/* set -> "set" id "equal" "to" (id | num) */
void set(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "set");
	
    struct entry *firstArg = search(*st, tk->val);
	expectName(str, tk, id);
	
	expectVal(str, tk, "equal");
	expectVal(str, tk, "to");
    
    if(tk->name == id)
    {   
        struct entry *secondArg = search(*st, tk->val);
        expectName(str, tk, tk->name);
        *instList = emitInst(*instList, instListSize, op_set, &(firstArg->val), &(secondArg->val), NULL);
    } else if(tk->name == num)
    {
        int *c = malloc(sizeof(int)); /* constant value temporary */
        *c = atoi(tk->val);
		*instList = emitInst(*instList, instListSize, op_set, &(firstArg->val), c, NULL);
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
}

/* let -> "let" id "equal" (id | num) */
void let(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "let");
	
	char *temp = tk->val;
	expectName(str, tk, id);
	
	expectVal(str, tk, "equal");
    
    if(tk->name == id)
    {
        struct entry *tempEntry = search(*st, tk->val);
        st->tb[st->top] = createEntry(st->tb[st->top], temp, tempEntry->val);
        expectName(str, tk, tk->name);
    } else if(tk->name == num)
    {
        st->tb[st->top] = createEntry(st->tb[st->top], temp, atoi(tk->val)); /* !!! ADD: definitely check size before hand so there is no overflow !!! */
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
}

/* ifst -> "if" id cond (id | num) "then" newlines block "end" */
void ifst(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "if");
    
    struct entry *firstArg = search(*st, tk->val);
    expectName(str, tk, id);
    
    enum instType cnd = cond(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    int tempLabel = *label;
    (*label)++;
    
    if(tk->name == id)
    {   
        struct entry *secondArg = search(*st, tk->val);
        expectName(str, tk, tk->name);
        *instList = emitInst(*instList, instListSize, cnd, &(firstArg->val), &(secondArg->val), &tempLabel);
    } else if(tk->name == num)
    {
        int *c = malloc(sizeof(int)); /* constant value temporary */
        *c = atoi(tk->val);
        *instList = emitInst(*instList, instListSize, cnd, &(firstArg->val), c, &tempLabel);
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "then");
    
    *st = pushTablePtr(*st);
    
    newlines(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    block(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    expectVal(str, tk, "end");
    
    *instList= emitInst(*instList, instListSize, t_label, &tempLabel, NULL, NULL);
    
    *st = popTablePtr(*st, saveTb, saveTableSize);
}

/* whilest -> "while" id cond (id | num) "do" newlines block "end" "while" */
void whilest(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    expectVal(str, tk, "while");
    
    struct entry *firstArg = search(*st, tk->val);
    expectName(str, tk, id);
    
    enum instType cnd = cond(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    
	int firstTempLabel = *label;
    (*label)++;
    int secondTempLabel = *label;
    (*label)++;
    
    if(tk->name == id)
    {   
        struct entry *secondArg = search(*st, tk->val);
        expectName(str, tk, tk->name);
        *instList = emitInst(*instList, instListSize, t_label, &firstTempLabel, NULL, NULL);
        *instList = emitInst(*instList, instListSize, cnd, &(firstArg->val), &(secondArg->val), &secondTempLabel);
    } else if(tk->name == num)
    {
        int *c = malloc(sizeof(int)); /* constant value temporary */
        *c = atoi(tk->val);
		*instList = emitInst(*instList, instListSize, t_label, &firstTempLabel, NULL, NULL);
        *instList = emitInst(*instList, instListSize, cnd, &(firstArg->val), c, &secondTempLabel);
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "do");
    
    *st = pushTablePtr(*st);
    
    newlines(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    block(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    expectVal(str, tk, "end");
    expectVal(str, tk, "while");
    
    *instList = emitInst(*instList, instListSize, op_jmp, &firstTempLabel, NULL, NULL);
    *instList = emitInst(*instList, instListSize, t_label, &secondTempLabel, NULL, NULL);
    
    *st = popTablePtr(*st, saveTb, saveTableSize);
}

/* cond -> "islessthan" | "isgreaterthan" | "isequalto" */
enum instType cond(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    if(strcmp(tk->val, "islessthan") == 0)
    {
        expectVal(str, tk, tk->val);
		return op_jge;
    } else if(strcmp(tk->val, "isgreaterthan") == 0)
	{
		expectVal(str, tk, tk->val);
		return op_jle;
	} else if(strcmp(tk->val, "isequalto") == 0)
	{
		expectVal(str, tk, tk->val);
		return op_jne;
	} else
    {
        printf("ERROR: malformed conditional\n");
        exit(-1);
    }
}

/* newlines -> "\n" newlines | e */
void newlines(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    if(tk->name == newline)
    {
        expectName(str, tk, newline);
        newlines(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    } else
    {
        
    }
}

void parse(char **str, struct token *tk, struct stack *st, struct table ***saveTb, int *saveTableSize, struct inst **instList, int *instListSize, int *label)
{
    *st = pushTablePtr(*st);
    
    newlines(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    block(str, tk, st, saveTb, saveTableSize, instList, instListSize, label);
    expectName(str, tk, EOI);
    
    *st = popTablePtr(*st, saveTb, saveTableSize);
}

void interpret(struct inst *instList, int instListSize)
{
    int i;
    for(i = 0; i<instListSize; i++)
    {
        switch(instList[i].type)
        {
            case op_add:
                *(instList[i].arg2) += *(instList[i].arg1);
            break;
            
            case op_subtract:
                *(instList[i].arg2) -= *(instList[i].arg1);
            break;
            
            case op_mult:
                *(instList[i].arg1) *= *(instList[i].arg2);
            break;
			
			case op_set:
				*(instList[i].arg1) = *(instList[i].arg2);
			break;
            
            case op_move:
                printf("%d\n", *(instList[i].arg1));
            break;
            
            case op_jle:
                if(*(instList[i].arg1) <= *(instList[i].arg2))
                {
                    int labelNumTemp = *(instList[i].arg3);
                    i = 0;
                    while(1)
                    {
                        if(instList[i].type == t_label)
                        {
                            if(*(instList[i].arg1) == labelNumTemp) break;
                        }
						i++;
                    }
                }
            break;
            
            case op_jge:
                if(*(instList[i].arg1) >= *(instList[i].arg2))
                {
                    int labelNumTemp = *(instList[i].arg3);
                    i = 0;
                    while(1)
                    {
                        if(instList[i].type == t_label)
                        {
                            if(*(instList[i].arg1) == labelNumTemp) break;
                        }
						i++;
                    }
                }
            break;
            
            case op_jne:
                if(*(instList[i].arg1) != *(instList[i].arg2))
                {
                    int labelNumTemp = *(instList[i].arg3);
                    i = 0;
                    while(1)
                    {
                        if(instList[i].type == t_label)
                        {
                            if(*(instList[i].arg1) == labelNumTemp) break;
                        }
						i++;
                    }
                }
            break;
			
			case op_jmp:
				int labelNumTemp = *(instList[i].arg1);
                i = 0;
				while(1)
				{
					if(instList[i].type == t_label)
					{
						if(*(instList[i].arg1) == labelNumTemp) break;
					}
					i++;
				}
			break;
        }
    }
}

int main(void)
{
    char *str = "let a equal 3 \
				 while a islessthan 5 do \
					move turtle forward a px \
					add 1 to a \
				 end while \
				 move turtle forward a px";
				
    struct token tk = getNextToken(&str); /* get first token */
    
    /* symbol table stack init */
    struct stack st;
    st.tb = NULL;
    st.top = -1;
    
    /* table of saved pointers for deallocation after
       values are popped off the stack */
    struct table **saveTb = NULL;
    int saveTableSize = 0;
    
    struct inst *instList = NULL;
    int instListSize = 0;
    
    int label = 0;
    
    parse(&str, &tk, &st, &saveTb, &saveTableSize, &instList, &instListSize, &label);
	
	/*int i;
	for(i = 0; i<instListSize; i++)
	{
		printf("%d ", instList[i].type);
		if(instList[i].arg1 != NULL) printf("%d ", *(instList[i].arg1));
		if(instList[i].arg2 != NULL) printf("%d ", *(instList[i].arg2));
		if(instList[i].arg3 != NULL) printf("%d ", *(instList[i].arg3));
		printf("\n");
	}*/
	
    interpret(instList, instListSize);
	
    int j;
    for(j = 0; j<saveTableSize; j++) free(saveTb[j]);
    free(saveTb);
	
	getchar();
	
    return 0;
}