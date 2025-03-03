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

struct dlinkedlist
{
    struct dlinkedlist *prev;
    struct table tb;
    struct dlinkedlist *next;
};

void createTbEntry(struct table *tb, char *id, int val)
{
    tb->entry = realloc(tb->entry, (tb->size+1) * sizeof(struct entry));
    tb->entry[tb->size].id = calloc(1, 1);
    strcat(tb->entry[tb->size].id, id); /* ERROR!!! BOUND OVERFLOW HERE */
    tb->entry[tb->size].val = val;
    tb->size++;
}

struct dlinkedlist * createTb(void)
{
    struct dlinkedlist *tblist = malloc(sizeof(struct dlinkedlist));
    tblist->tb.size = 0;
    
    return tblist;
}

/*
    block -> statement newlines block | e
    statement -> op | if | while
    oper -> add | sub | mult | set
    add -> "add" (id | num) "to" id
    sub -> "subtract" (id | num) "from" id
    mult -> "multiply" id "by" (id | num)
    set -> "set" id "equal" "to" (id | num)
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
void set();
void ifst();
void whilest();
void cond();
void newlines();

/* block -> statement newlines block | e */
void block(char **str, struct token *tk)
{
    if(strcmp(tk->val, "add") == 0      ||
       strcmp(tk->val, "subtract") == 0 ||
       strcmp(tk->val, "multiply") == 0 ||
       strcmp(tk->val, "set") == 0      ||
       strcmp(tk->val, "if") == 0       ||
       strcmp(tk->val, "while") == 0)
    {
        statement(str, tk);
        newlines(str, tk);
        block(str, tk);
    }
}

/* statement -> op | if | while */
void statement(char **str, struct token *tk)
{
    if(strcmp(tk->val, "add") == 0  ||
       strcmp(tk->val, "subtract") == 0  ||
       strcmp(tk->val, "multiply") == 0 ||
       strcmp(tk->val, "set") == 0)
    {
        oper(str, tk);
    } else if(strcmp(tk->val, "if") == 0)
    {
        ifst(str, tk); /* if statement */
    } else if(strcmp(tk->val, "while") == 0)
    {
        whilest(str, tk); /* while statement */
    } else
    {
        printf("ERROR: \"%s\" is not a valid beginning of a statement\n", tk->val);
        exit(-1);
    }
}

/* oper -> add | sub | mult | set */
void oper(char **str, struct token *tk)
{
    if(strcmp(tk->val, "add") == 0)
    {
        add(str, tk);
    } else if(strcmp(tk->val, "subtract") == 0)
    {
        sub(str, tk);
    } else if(strcmp(tk->val, "multiply") == 0)
    {
        mult(str, tk);
    } else if(strcmp(tk->val, "set") == 0)
    {
        set(str, tk);
    } else
    {
        printf("ERROR: malformed operation\n");
        exit(-1);
    }
}

/* add -> "add" (id | num) "to" id */
void add(char **str, struct token *tk)
{
    expectVal(str, tk, "add");
    
    if(tk->name == id || tk->name == num)
    {
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "to");
    expectName(str, tk, id);
}

/* sub -> "subtract" (id | num) "from" id */
void sub(char **str, struct token *tk)
{;
    expectVal(str, tk, "subtract");
    
    if(tk->name == id || tk->name == num)
    {
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "from");
    expectName(str, tk, id);
}

/* mult -> "multiply" id "by" (id | num) */
void mult(char **str, struct token *tk)
{
    expectVal(str, tk, "multiply");
    
    expectName(str, tk, id);
    
    expectVal(str, tk, "by");
    
    if(tk->name == id || tk->name == num)
    {
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
}

/* set -> "set" id "equal" "to" (id | num) */
void set(char **str, struct token *tk)
{
    expectVal(str, tk, "set");
	
	expectName(str, tk, id);
	
	expectVal(str, tk, "equal");
	expectVal(str, tk, "to");
    
    if(tk->name == id || tk->name == num)
    {
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
}

/* ifst -> "if" id cond (id | num) "then" newlines block "end" */
void ifst(char **str, struct token *tk)
{
    expectVal(str, tk, "if");
    expectName(str, tk, id);
    cond(str, tk);
    
    if(tk->name == id || tk->name == num)
    {
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "then");
    newlines(str, tk);
    block(str, tk);
    expectVal(str, tk, "end");
}

/* whilest -> "while" id cond (id | num) "do" newlines block "end" "while" */
void whilest(char **str, struct token *tk)
{
    expectVal(str, tk, "while");
    expectName(str, tk, id);
    cond(str, tk);
    
    if(tk->name == id || tk->name == num)
    {
        expectName(str, tk, tk->name);
    } else
    {
        printf("ERROR: expected id or num\n");
        exit(-1);
    }
    
    expectVal(str, tk, "do");
    newlines(str, tk);
    block(str, tk);
    expectVal(str, tk, "end");
    expectVal(str, tk, "while");
}

/* cond -> "islessthan" | "isgreaterthan" | "isequalto" */
void cond(char **str, struct token *tk)
{
    if(strcmp(tk->val, "islessthan") == 0 ||
       strcmp(tk->val, "isgreaterthan") == 0 ||
       strcmp(tk->val, "isequalto") == 0)
    {
        expectVal(str, tk, tk->val);
    } else
    {
        printf("ERROR: malformed conditional\n");
        exit(-1);
    }
}

/* newlines -> "\n" newlines | e */
void newlines(char **str, struct token *tk)
{
    if(tk->name == newline)
    {
        expectName(str, tk, newline);
        newlines(str, tk);
    } else
    {
        
    }
}

void parse(char **str, struct token *tk)
{
    newlines(str, tk);
    block(str, tk);
    expectName(str, tk, EOI);
}

int main(void)
{
    char *str = "if x islessthan 5 then\n\n\n add 3 to x\n\n\n subtract 1 from x\n\n\n end";
    struct token tk = getNextToken(&str);
    parse(&str, &tk);
	
    return 0;
}