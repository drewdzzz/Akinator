#include "bin-tree.hpp"
#include <cassert>
static const int BUFSIZE = 256;

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

    void read_tree (char* input_file)
    {
        FILE* stream = fopen (input_file, "r");
        assert (stream);
        read_undertree (stream, head);
        fclose (stream);
    }

    void write_tree (char* output_file)
    {
        FILE* stream = fopen (output_file, "w");
        assert (stream);
        write_undertree (stream, head);
        fclose (stream);
    }
    friend void go_lower_and_ask (Akinator_tree &tree, Akinator_tree::Node_t *node);
    friend void add_character (Akinator_tree &tree, Akinator_tree::Node_t *node);
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

void add_character (Akinator_tree &tree, Akinator_tree::Node_t *node)
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
    node -> data = userInput;
}


void go_lower_and_ask (Akinator_tree &tree, Akinator_tree::Node_t *node)
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
        }
    }
}

void game_logic (Akinator_tree &tree)
{
    printf ("Здравствуй, мой друг. Сыграем в акинатора?\n");

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


int main ()
{
    setlocale (LC_ALL, "rus");
    Akinator_tree tree;
    tree.read_tree((char*)"tree-base.txt");
    game_logic (tree);
    tree.draw ((char*)"open");
    tree.write_tree((char*)"tree-base.txt");

    return 0;
}
