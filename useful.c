#include <stdio.h>
#include <stdlib.h>

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
        if(tb->entry[i].id == str)
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

struct stack push(struct stack st)
{
    st.top++;
    st.tb = realloc(st.tb, (st.top+1) * sizeof(struct table *));
    st.tb[st.top] = malloc(sizeof(struct table));
    
    st.tb[st.top]->entry = NULL;
    st.tb[st.top]->size = 0;
    
    return st;
}

int main(void) 
{
    struct stack st;
    st.top = -1;
    st.tb = NULL;
    st = push(st);
    
    st.tb[st.top] = createEntry(st.tb[st.top], "test", 123);
    
    printf("%s, %d\n", st.tb[st.top]->entry[0].id, st.tb[st.top]->entry[0].val);

    return 0;
}