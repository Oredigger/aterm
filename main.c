#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum token_id token_id_t;

enum token_id
{
    IDENTIFIER  = 0,
    KEYWORD     = 1,
    EQUAL       = 2,
    LEFT_PAREN  = 3,
    RIGHT_PAREN = 4,
    OPERATOR    = 5,
    COMMA       = 6,
    WHITESPACE  = 7,
    VALUE       = 8,
    NA          = 9
};

typedef struct token token_t;

struct token
{
    uint8_t tok_val;
    char   *str_val;
};

token_t new_token_t(void)
{
    return (token_t){.tok_val = NA, .str_val = NULL};
}

void set_tok_val(token_t *tok_ptr, token_id_t tok_val)
{
    if (!tok_ptr)
    {
        return;
    }

    tok_ptr->tok_val = tok_val;
}

void set_str_val(token_t *tok_ptr, char *str_val)
{
    if (!tok_ptr)
    {
        return;
    }

    size_t len = strlen(str_val);
    char  *cpy = (char *) calloc(len + 1, sizeof(char));
    
    if (cpy)
    {
        memcpy(cpy, str_val, len);
        memset(cpy + len, 0, 1);
    }

    tok_ptr->str_val = cpy;
}

token_t copy_token_t(token_t *tok_ptr)
{
    token_t copy = new_token_t();

    if (tok_ptr)
    {
        set_tok_val(&copy, tok_ptr->tok_val);
        set_str_val(&copy, tok_ptr->str_val);
    }

    return copy;
}

void print_token_t(token_t *tok_ptr)
{
    if (!tok_ptr)
    {
        return;
    }

    switch(tok_ptr->tok_val)
    {
        case IDENTIFIER:
            printf("IDENTIFIER,%s\n", tok_ptr->str_val);
            break;
        case KEYWORD:
            printf("KEYWORD,%s\n", tok_ptr->str_val);
            break;
        case EQUAL:
            printf("EQUAL,%s\n", tok_ptr->str_val);
            break;
        case LEFT_PAREN:
            printf("LEFT_PAREN,%s\n", tok_ptr->str_val);
            break;
        case RIGHT_PAREN:
            printf("RIGHT_PAREN,%s\n", tok_ptr->str_val);
            break;
        case OPERATOR:
            printf("OPERATOR,%s\n", tok_ptr->str_val);
            break;
        case COMMA:
            printf("COMMA,%s\n", tok_ptr->str_val);
            break;
        case WHITESPACE:
            printf("WHITESPACE,%s\n", tok_ptr->str_val);
            break;
        case VALUE:
            printf("VALUE,%s\n", tok_ptr->str_val);
            break;
        case NA:
            printf("NA,%s\n", tok_ptr->str_val);
            break;
        default:
            break;
    }
}

void free_token_t(token_t *tok_ptr)
{
    if (!tok_ptr)
    {
        return;
    }

    if (tok_ptr->str_val)
    {
        free(tok_ptr->str_val);
    }
}

typedef enum val_type val_type_t;

enum val_type
{
    TOKEN_T, 
    UINT8_T, 
    UNKNOWN
};

typedef enum gen_stack_err gen_stack_err_t;

enum gen_stack_err
{
    CS_NO_ERROR, 
    CS_NULL_HEAD_ERROR, 
    CS_MEM_ALLOC_ERROR,
    CS_NULL_VALUE_ERROR
};

typedef struct gen_stack gen_stack_t;

struct gen_stack
{
    union
    {
        token_t token_val;
        uint8_t uint8_val;
    };

    val_type_t type;
    size_t pos;
    gen_stack_t *next;
};

gen_stack_err_t push_to_gen_stack(gen_stack_t **head_node, void *val_ptr, val_type_t val_type)
{
    if (!head_node)
    {
        return CS_NULL_HEAD_ERROR;
    }

    if (!val_ptr || 
       ((*head_node) && 
        (*head_node)->type != val_type))
    {
        return CS_NULL_VALUE_ERROR;
    }

    gen_stack_t *new_node = (gen_stack_t *) malloc(sizeof(gen_stack_t));
    gen_stack_err_t err = CS_MEM_ALLOC_ERROR;
    
    if (new_node)
    {
        new_node->pos  = *head_node ? (*head_node)->pos + 1 : 0;
        new_node->next = *head_node;
        new_node->type =  val_type;

        switch (new_node->type)
        {
            case TOKEN_T:
                new_node->token_val = *((token_t *) val_ptr);
                break;
            case UINT8_T:
                new_node->uint8_val = *((uint8_t *) val_ptr);
                break;
            default:
                new_node->type = UNKNOWN;
                break;
        }

        err = CS_NO_ERROR;
    }

    *head_node = new_node; 
    return err;
}

gen_stack_err_t peek_gen_stack(gen_stack_t *head_node, void *val_ptr, val_type_t val_type)
{
    if (!head_node)
    {
        return CS_NULL_HEAD_ERROR;
    }

    if (!val_ptr || 
        (head_node->type != val_type))
    {
        return CS_NULL_VALUE_ERROR;
    }

    switch (head_node->type)
    {
        case TOKEN_T:
            *((token_t *) val_ptr) = copy_token_t(&head_node->token_val);
            break;
        case UINT8_T:
            *((uint8_t *) val_ptr) = head_node->uint8_val;
            break;   
        default:
            break;
    }

    return CS_NO_ERROR;
}

gen_stack_err_t pop_one_gen_stack(gen_stack_t **head_node, void *val_ptr, val_type_t val_type)
{
    if (!head_node || !*head_node)
    {
        return CS_NULL_HEAD_ERROR;
    }

    gen_stack_t *new_node = (*head_node)->next;

    if (val_ptr)
    {
        peek_gen_stack(*head_node, val_ptr, val_type);
    }

    if ((*head_node)->type == TOKEN_T)
    {
        free_token_t(&(*head_node)->token_val);
    }

    free(*head_node);
    *head_node = new_node;

    return CS_NO_ERROR;
}

gen_stack_err_t pop_all_gen_stack(gen_stack_t **head_node)
{
    if (!head_node)
    {
        return CS_NULL_HEAD_ERROR;
    }

    if (*head_node)
    {
        if ((*head_node)->type == TOKEN_T)
        {
            free_token_t(&(*head_node)->token_val);
        }

        pop_all_gen_stack(&((*head_node)->next));

        free(*head_node);
        *head_node = NULL;
    }

    return CS_NO_ERROR;
}

gen_stack_err_t free_gen_stack(gen_stack_t **head_node)
{
    if (!head_node)
    {
        return CS_NULL_HEAD_ERROR;
    }
    
    if (*head_node)
    {
        pop_all_gen_stack(head_node);
        free(*head_node);
       *head_node = NULL;
    }

    return CS_NO_ERROR;
}

void to_string(gen_stack_t **head_node, char **str)
{
    if (!(head_node && *head_node) ||
         (*head_node)->type != UINT8_T)
    {
        return;
    }

    *str = (char *) calloc((*head_node)->pos + 2, sizeof(char));
    
    if (*str)
    {
        size_t offset = (*head_node)->pos + 1;

        for (int i = 0; i < offset; i++)
        {
            pop_one_gen_stack(head_node, *str + offset - i - 1, UINT8_T);
        }

        memset(*str + offset, 0, 1);
    }
}

char *get_stdin(gen_stack_t **head_node)
{
    if (!head_node)
    {
        return NULL;
    }

    gen_stack_err_t err = CS_NO_ERROR;
    
    bool status = true;
    uint8_t val = 0;

    while (status)
    {
        val = fgetc(stdin);

        if (val != EOF && val != '\n')
        {
            err = push_to_gen_stack(head_node, &val, UINT8_T);
            status = (err == CS_NO_ERROR);
        }
        else
        {
            status = 0;
        }
    }
 
    char *str = NULL;

    if (err == CS_NO_ERROR)
    {
        to_string(head_node, &str);
    }

    return str;
}

bool is_valid_var_name(char *str)
{
    if (!str)
    {
        return false;
    }

    bool violate = isdigit(*str) > 0;

    while (*str && !violate)
    {
        if (!isalpha(*str) &&
            !isdigit(*str) &&
            *str != '_')
        {
            violate = true;
        }

        str++;
    }

    return !violate;
}

bool is_keyword(char *str)
{
    if (!str)
    {
        return false;
    }

    return strcmp(str, "num") == 0 ||
           strcmp(str, "run") == 0 ||
           strcmp(str, "rdn") == 0 ||
           strcmp(str, "tim") == 0;
}

void token_push(gen_stack_t **tokens, char *str, token_id_t token_id)
{
    token_t tok = new_token_t();
    
    set_tok_val(&tok, token_id);
    set_str_val(&tok, str);
    push_to_gen_stack(tokens, &tok, TOKEN_T);
}

void tokenize(char *str)
{
    if (!str)
    {
        return;
    }

    const char  *list_of_ops = "%%^=-+*\\/<>";

    gen_stack_t    *tokens = NULL;
    gen_stack_t *head_node = NULL;

    char    *token = NULL;
    bool loop_flag = true;

    while (loop_flag)
    {
        char curr = *str;
        str++;

        if (isalpha(curr) || isdigit(curr))
        {
            push_to_gen_stack(&head_node, &curr, UINT8_T);
            continue;
        }
            
        to_string(&head_node, &token);

        if (token)
        {
            uint8_t tmp = (is_keyword(token)) ? KEYWORD : IDENTIFIER;
            token_push(&tokens, token, tmp);

            free(token);
            token = NULL;
        }
        
        switch (curr)
        {
            case  '(':
                token_push(&tokens, "(", LEFT_PAREN);
                break;
            case  ')':
                token_push(&tokens, ")", RIGHT_PAREN);
                break;
            case  '=':
                token_push(&tokens, "=", EQUAL);
                break;
            case  ',':
                token_push(&tokens, ",", COMMA);
                break;
            case  ' ':
                break;
            case '\0':
                loop_flag = false;
                break;
            default:
                char tmp[1] = {curr};
                token_push(&tokens, tmp, strchr(list_of_ops, curr) ? OPERATOR : NA);
                break;
        }
    }

    free_gen_stack(&head_node);
    free_gen_stack(&tokens);
}

int main(void)
{
    bool quit_flag = false;
    gen_stack_t *head_node = NULL;

    while (!quit_flag)
    {
        printf("andy> ");
        char *resp = get_stdin(&head_node);
        tokenize(resp);
        quit_flag = resp && (strcmp(resp, "quit") == 0);
        free(resp);
    }

    free_gen_stack(&head_node);
    return 0;
}

/*
    while (tokens)
    {
        token_t tmp;
        pop_one_gen_stack(&tokens, &tmp, TOKEN_T);
        print_token_t(&tmp);
        free_token_t(&tmp);
    }
 */