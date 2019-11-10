#include "bin-tree.hpp"
#include <cassert>
static const int BUFSIZE = 256;

class Akinator_tree : public Tree_t<char*>
{
    char symb = 0;
    int read_count = 0;
    char buf[BUFSIZE] = "";
    void read_undertree (FILE* stream, Node_t *node)
    {
        assert (stream);
        assert (node);

        fscanf (stream," %*[ {'] ");
        fscanf (stream,"%[^']%n", buf, &read_count);
        node -> data = (char*) calloc (read_count+1, sizeof (char));
        strcpy (node -> data, buf);
        fscanf (stream,"' ");

        fscanf (stream, " %c ", &symb);
        switch (symb)
        {
            case '{': make_left (node, (char*)""); read_undertree (stream, node -> left); break;
            case '}': return;  break;
            default : return; abort(); break;
        }
        fscanf (stream, " %c ", &symb);
        switch (symb)
        {
            case '{': make_right(node, (char*)"");read_undertree (stream, node -> right); break;
            case '}': return; break;
            default : return; abort(); break;
        }
    }

    /*void define_for_draw() override
    {


    }*/
public:

    void read_tree (char* input_file)
    {
        FILE* stream = fopen (input_file, "r");
        assert (stream);
        read_undertree (stream, head);
        fclose (stream);
    }
};

int main ()
{
    Akinator_tree tree;
    tree.read_tree((char*)"tree-base.txt");
    tree.draw ((char*)"open");
    return 0;
}
