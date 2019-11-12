#include "bin-tree.hpp"
#include <cassert>
#include <unistd.h>

static const int BUFSIZE = 256;
static const int INPUTSIZE = 256;
static const char* TREE_INPUT = "tree-base.txt";
static const char* TREE_OUTPUT = "tree-base.txt";

static char userInput[BUFSIZE] = "";

class Akinator_tree : public Tree_t<char*>
{
    char symb = 0;
    int read_count = 0;
    char buf[BUFSIZE] = "";
    bool YES_flag = false;

    void read_undertree (FILE* stream, Node_t *node)
    {
        assert (stream);
        assert (node);

        fscanf (stream," %*[{' ] ");
        fscanf (stream,"%[^'{}]%n", buf, &read_count);
        node -> data = (char*) calloc (read_count+1, sizeof (char));
        strcpy (node -> data, buf);
        fscanf (stream,"' ");

        fscanf (stream, " %c ", &symb);
        switch (symb)
        {
            case '{': make_left (node, (char*)""); read_undertree (stream, node -> left); break;
            case '}': return; break;
            default : fprintf (stderr, "Wrong syntax. Waited for '{' or '}'"); abort(); break;
        }
        fscanf (stream, " %c ", &symb);
        switch (symb)
        {
            case '{': make_right (node, (char*)""); read_undertree (stream, node -> right); break;
            case '}': return; break;
            default : fprintf (stderr, "Wrong syntax. Waited for '{' or '}'"); abort(); break;
        }
        fscanf (stream, " %c ", &symb);
        switch (symb)
        {
            case '{': fprintf (stderr, "Wrong syntax. Too much descedants (node can have only 2)"); abort(); break;
            case '}': return; break;
            default : fprintf (stderr, "Wrong syntax. Waited for '{' or '}'"); abort(); break;
        }

    }

    void write_undertree (FILE* stream, Node_t *node)
    {
        assert (stream);
        assert (node);

        fprintf (stream, "{ '");
        write_data(stream, node -> data);
        fprintf (stream, "' ");
        if (node -> left)
            write_undertree (stream, node -> left);
        if (node -> right)
            write_undertree (stream, node -> right);
        fprintf (stream, "}");
    }

    void draw_nodes (FILE* stream, Node_t *node) override
    {
        if (node != head)
        {
            fprintf (stream, "\"tree_node%p\"\n", node);
            if (YES_flag)
                fprintf (stream, "[label = \"YES\", color = \"blue\"]");
            else
                fprintf (stream, "[label = \"NO\", color = \"red\"]");
        }
        if (node -> left)
        {
            fprintf (stream,"\"tree_node%p\" -> ", node);
            YES_flag = false;
            draw_nodes (stream, node -> left);
        }
        if (node -> right)
        {
            fprintf (stream,"\"tree_node%p\" -> ", node);
            YES_flag = true;
            draw_nodes (stream, node -> right);
        }
    }

public:

    void read_tree (const char* input_file)
    {
        FILE* stream = fopen (input_file, "r");
        assert (stream);
        read_undertree (stream, head);
        fclose (stream);
    }

    void write_tree (const char* output_file)
    {
        FILE* stream = fopen (output_file, "w");
        assert (stream);
        write_undertree (stream, head);
        fclose (stream);
    }

    bool deep_search (Node_t *node, char* character, char* characteristics, long &depth)
    {
        depth++;
        if (! node -> left && ! node -> right)
            if ( ! strcmp (node -> data, character) )
            {
                return true;
            }
            else
            {
                depth--;
                return false;
            }
        if ( node -> left)
        {
            characteristics [depth] = 0;
            if ( deep_search ( node -> left, character, characteristics, depth ) )
                return true;
        }
        if ( node -> right)
        {
            characteristics [depth] = 1;
            if ( deep_search ( node -> right, character, characteristics, depth) )
                return true;
        }
        depth--;
        return false;
    }


    friend void go_lower_and_ask (Akinator_tree &tree, Akinator_tree::Node_t* &node);
    friend void add_character (Akinator_tree &tree, Akinator_tree::Node_t* &node);
    friend void comporation_mode (Akinator_tree &tree);
};



bool check_answer ()
{
    printf ("\nЯ угадал?\n");
    {
        scanf ("%s", userInput);

        if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
            return true;
        else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
            return false;
        else
            printf ("Неправильный ответ, попробуй ещё\n");
    }
}

void add_character (Akinator_tree &tree, Akinator_tree::Node_t* &node)
{
    int read_count;
    printf (":(\n"
            "Хорошо, кто это был?\n");
    scanf (" %[^\n]%n%", userInput, &read_count );
    tree.make_right (node, nullptr);
    node -> right -> data = (char*) calloc ( tree.read_count + 1, sizeof (char) );
    strcpy (node -> right -> data, userInput);

    tree.make_left (node, nullptr);
    node -> left -> data = (char*) calloc ( tree.read_count + 1, sizeof (char) );
    strcpy (node -> left -> data, node -> data);

    printf ("И чем же ваш персонаж отличается от моего?\n");
    scanf (" %[^\n]%n%", userInput, &read_count );
    node -> data = (char*)realloc (node -> data, (tree.read_count + 1) * sizeof (char) );
    strcpy (node -> data, userInput);
}


void go_lower_and_ask (Akinator_tree &tree, Akinator_tree::Node_t* &node)
{
    if ( node -> right && node -> left )
    {
        printf ("\033[1;36m");
        tree.write_data (stdout, node -> data);
        printf ("?\n");
        printf ("\033[0m");
        while (1)
        {
            scanf ("%s", userInput);

            if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
            {
                go_lower_and_ask (tree, node -> right);
                break;
            }
            else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
            {
                go_lower_and_ask (tree, node -> left);
                break;
            }
            else
            {
                printf ("Неправильный ответ, попробуй ещё\n");
            }
        }
    }
    else
    {
        printf ("\033[1;32m");
        printf ("%s", node -> data);
        printf ("\033[0m");
        if ( ! check_answer() )
        {
            add_character (tree, node);
            #ifndef FREE_FROM_BASE_TREE
                tree.write_tree (TREE_OUTPUT);
            #endif
        }
    }
}

void find_mode (Akinator_tree &tree)
{
    printf ("Здравствуй, мой друг. Ты точно готов поразиться моей гениальностью?\n");

    while (1)
    {
        scanf ("%s", userInput);
        if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
        {
            go_lower_and_ask (tree, tree.head);
            break;
        }
        else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
        {
            return;
        }
        else
        {
            printf ("Неправильный ответ, попробуй ещё\n");
        }
    }
}

Akinator_tree::Node_t* write_equals (Akinator_tree::Node_t *node, char* char1, char* char2, long &depth)
{
    Akinator_tree::Node_t* last_node = nullptr;
    depth++;
    if (char1 [depth] == char2 [depth])
    {
        printf ("\033[1;32m");
        if ( char1 [depth] == 0 )
            printf ("Не ");
        printf ("%s\n", node -> data);
        printf ("\033[0m");
    }
    else
        return node;
    if (char1 [depth] == char2 [depth])
        if ( char1 [depth] == 0 )
            last_node = write_equals (node -> left , char1, char2, depth);
        else
            last_node = write_equals (node -> right, char1, char2, depth);
    else
    return last_node;
}

void write_unequals (Akinator_tree::Node_t *node, char* charact, long depth)
{
    if ( ! node -> left && ! node -> right )
        return;

    printf ("\033[1;31m");
    if (charact [depth] == 0)
        printf ("Не ");
    printf ("%s\n", node -> data);
    printf ("\033[0m");
    if (charact [depth] == 0 )
    {
        write_unequals (node -> left, charact, depth+1);
    }
    if (charact [depth] == 1 )
    {
        write_unequals (node -> right, charact, depth+1);
    }
}

void write_comporation (Akinator_tree::Node_t *node, char* char1, char* char2, long &depth)
{
    printf ("\033[1;38m");
    printf ("\nЧем они похожи:\n");
    printf ("\033[0m");
    Akinator_tree::Node_t* first_unequal = write_equals (node, char1, char2, depth);
    printf ("\033[1;38m");
    printf ("\nПервый, в отличии от второго:\n");
    printf ("\033[0m");
    write_unequals (first_unequal, char1, depth);
    printf ("\033[1;38m");
    printf ("\nВторой, в отличии от первого:\n");
    printf ("\033[0m");
    write_unequals (first_unequal, char2, depth);
}

void comporation_mode (Akinator_tree &tree)
{
    char character1[256] ="";
    long depth = 0;
    char* characteristics1 = (char*) calloc (tree.node_counter, sizeof (char));
    char* characteristics2 = (char*) calloc (tree.node_counter, sizeof (char));

    printf ("Кого вы хотите сравнить?\n");
    scanf (" %[^\n]", userInput);
    if ( ! tree.deep_search (tree.head, userInput, characteristics1, depth) )
    {
        printf ("Такого персонажа нет:(\n");
        return;
    }
    strcpy (character1, userInput);
    printf ("С кем сравниваем?\n");
    scanf (" %[^\n]", userInput);
    if ( ! strcmp (character1, userInput) )
    {
        printf ("Так это один и тот же персонаж!");
        return;
    }

    depth = 0;
    if ( ! tree.deep_search (tree.head, userInput, characteristics2, depth) )
    {
        printf ("Такого персонажа нет:(\n");
        return;
    }
    depth = 0;
    write_comporation (tree.head, characteristics1, characteristics2, depth);
}

void menus (Akinator_tree &tree)
{
    char user_input[INPUTSIZE] = {};
    printf ("************************************\n");
    printf ("* Alawar inc.                      *\n");
    printf ("*         presents...              *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("************************************\n");
    sleep  (2);
    system ("clear");
    printf ("************************************\n");
    printf ("* Alawar inc.                      *\n");
    printf ("*         presents...              *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*         Another innovative game  *\n");
    printf ("************************************\n");
    sleep  (2);
    system ("clear");
    printf ("************************************\n");
    printf ("* Alawar inc.                      *\n");
    printf ("*         presents...              *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*             AKINATOR             *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*                                  *\n");
    printf ("*         Another innovative game  *\n");
    printf ("************************************\n");
    sleep  (2);
    system ("clear");

    while (1)
    {
        printf ("Выберите режим:\n");
        printf ("1. \033[1;34mЗагадай - ка\033[0m\n");
        printf ("2. \033[1;35mСравнение объектов\033[0m\n");
        printf ("3. \033[1;31mВыход\033[0m\n");
        printf ("4. \033[1;36mХочу посмотреть твоё дерево!\033[0m\n");
        printf ("(─‿‿─)\n\n\n\\\\Акинатор приехал с Казахстана, поэтому знает мало слов на нашем языке.\n"
                "Чтобы Акинатор наверняка понял тебя, старайся отвечать да (yes) или нет (no)\n\n");
        while (1)
        {
            scanf  (" %s", user_input);
            if ( user_input [0] == '1' )
            {
                system("clear");
                find_mode (tree);
                break;
            }
            else if ( user_input [0] == '2' )
            {
                system("clear");
                comporation_mode (tree);
                fflush (stdin);
                scanf ("%с", &userInput);
                break;
            }
            else if ( user_input [0] == '3' )
            {
                return;
            }
            else if ( user_input [0] == '4' )
            {
                system ("clear");
                tree.draw ("open");
                break;
            }
            else
            {
                printf ("Не понимаю, скажи ещё раз\n");
            }
        }
        system ("clear");
    }
}

int main ()
{
    setlocale (LC_ALL, "rus");
    Akinator_tree tree;
    tree.read_tree(TREE_INPUT);
    menus (tree);

    return 0;
}
