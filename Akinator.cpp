#include "bin-tree.hpp"
#include <cassert>
static const int BUFSIZE = 256;

char userInput[BUFSIZE] = "";

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

    friend void go_lower_and_ask (Akinator_tree tree, Akinator_tree::Node_t *node);
};


void go_lower_and_ask (Akinator_tree tree, Akinator_tree::Node_t *node)
{
    if ( node -> right && node -> left )
    {
        tree.write_data (stdout, node -> data);
        printf ("?\n");
        scanf ("%s", userInput);
        if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
            go_lower_and_ask (tree, node -> right);
        else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
            go_lower_and_ask (tree, node -> left);
        else
        {
            fprintf (stderr, "Wrong answer, try again\n");
            go_lower_and_ask (tree, node);
        }
    }
    else
        printf ("%s", node -> data);
}

void game_logic (Akinator_tree tree)
{
    printf ("Здравствуй, мой друг. Сыграем в акинатора? Предупреждаю, обыграть меня ооочень непростоб хе-хе;)\n");
    scanf ("%s", userInput);
    if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
        go_lower_and_ask (tree, tree.head);
    else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
        return;
    else
    {
        fprintf (stderr, "Wrong answer, try again\n");
        game_logic(tree);
    }
}


int main ()
{
    setlocale (LC_ALL, "rus");
    Akinator_tree tree;
    tree.read_tree((char*)"tree-base.txt");
    tree.draw ((char*)"open");
    FILE* stream = fopen ("out.txt", "w");
    tree.write_undertree (stream, tree.head);

    game_logic (tree);


    return 0;
}
