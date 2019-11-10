///@file
#ifndef INCLUDE_TREE_H

#define INCLUDE_TREE_H

#include <iostream>
#include <stdlib.h>
#include <cstring>

#define $p getchar()

///@brief Enum with error codes for using my binary tree
enum ERR_CODE
{
    OK = 0,
    ALLOC_ERROR = 1,
    IS_NOT_FREE = 2,
    NOT_EXIST = 3,
    NOT_LEAF = 4
};


///@brief Template class tree
template <typename T>
class Tree_t
{
public:
    ///@brief Node struct
    struct Node_t
    {
        T data = 0;
        Node_t *right = nullptr;
        Node_t *left = nullptr;

        Node_t (): right (nullptr), left (nullptr), data (0)
        {};
    } *head;

    ///@brief Constructor of Tree_t
    Tree_t ()
    {
        head = new Node_t;
    }

    ///@brief Destructor of Tree_t
    ~Tree_t ()
    {
        free_tree (head);
    }

    ///@brief Function sets data to node
    ///@return OK if data was set or NOT_EXIST if node doesn't exist
    ERR_CODE set_data (Node_t *node, T data)
    {
        if (!node) return NOT_EXIST;
        node -> data = data;
        return OK;
    }

    ///@brief Function creates a right descedant
    ///@return OK if descedant was created, NOT_EXIST if node doesn't exist, IS_NOT_FREE if descedant was created before
    ERR_CODE make_right (Node_t *node, T data)
    {
        if (!node)         return NOT_EXIST;
        if (node -> right) return IS_NOT_FREE;

        node -> right = new Node_t;
        node -> right -> data = data;
        return OK;
    }

    ///@brief Function creates a left descedant
    ///@return OK if descedant was created, NOT_EXIST if node doesn't exist, IS_NOT_FREE if descedant was created before
    ERR_CODE make_left (Node_t *node, T data)
    {
        if (!node)        return NOT_EXIST;
        if (node -> left) return IS_NOT_FREE;

        node -> left = new Node_t;
        node -> left -> data = data;
        return OK;
    }

    ///@brief Delets a leaf
    ///@return OK if leaf was deletead, NOT_EXIST if node doesn't exist, NOT_LEAF if node is not leaf
    ERR_CODE delete_leaf (Node_t **node)
    {
        if (!node)                               return NOT_EXIST;
        if ((*node) -> right || (*node) -> left) return NOT_LEAF;
        free (*node);
        *node = nullptr;
    }

    ///@brief VIRTUAL function for definition of nodes for dump
    virtual void define_for_draw (FILE* stream, Node_t *node, bool dump)
    {
        fprintf (stream, "\"tree_node%p\" [label = \"", node);
        write_data (stream, node -> data);
        if (dump)
            fprintf (stream," \n Address: %p\n Left: %p \n Right: %p", node, node -> left, node -> right);
        fprintf (stream,"\"]\n");

        if (node -> left)
        {
            define_for_draw (stream, node -> left, dump);
        }
        if (node -> right)
        {
            define_for_draw (stream, node -> right, dump);
        }
    }

    ///@brief VIRTUAL function for drawing of nodes
    virtual void draw_nodes (FILE* stream, Node_t *node)
    {
        fprintf (stream, "\"tree_node%p\"\n", node);
        if (node -> left)
        {
            fprintf (stream,"\"tree_node%p\" -> ", node);
            draw_nodes (stream, node -> left);
        }
        if (node -> right)
        {
            fprintf (stream,"\"tree_node%p\" -> ", node);
            draw_nodes (stream, node -> right);
        }

    }


    ///@brief dump of tree including only data
    ///@note Use "open" as an arguement for instant opening of png file
    void draw ()
    {
        FILE* stream = fopen ("Tree_draw.dot", "w");
        fprintf (stream, "digraph Tree{\n");
        define_for_draw (stream, head, false);
        draw_nodes (stream, head);
        fprintf (stream,"\n}");
        fclose (stream);
        system  ("dot -Tpng Tree_draw.dot -o tree_draw.png");
    }

    void draw (char* arg)
    {
        draw ();
        if ( strcmp (arg, "open") == 0 )
            system ("xdg-open tree_draw.png");
    }

    ///@brief dump of tree including: data, address of node and addresses of children
    ///@note Use "open" as an arguement for instant opening of png file
    void dump ()
    {
        FILE* stream = fopen ("Tree_dump.dot", "w");
        fprintf (stream, "digraph Tree{\n");
        define_for_draw (stream, head, true);
        draw_nodes (stream, head);
        fprintf (stream,"\n}");
        fclose (stream);
        system  ("dot -Tpng Tree_dump.dot -o tree_dump.png");
    }

    void dump (char* arg)
    {
        dump ();
        if ( strcmp (arg, "open") == 0 )
            system ("xdg-open tree_dump.png");
    }

private:

    ///@brief A function for destructor... and btw it's private so it's NOT UR BUSINESS
    void free_tree (Node_t *tree)
    {
        if (tree)
        {
            free_tree (tree -> left );
            free_tree (tree -> right);
            free (tree);
        }
    }

    //-----------------------------------------------------------------------------------------------------
    //Overloads of print function for dumping or drawing
    //-----------------------------------------------------------------------------------------------------
    ///@brief Overloads of print function for dumping or drawing
    void write_data (FILE* stream, const int &value)
    {
        fprintf (stream, "%d", value);
    }

    void write_data (FILE* stream, const unsigned int &value)
    {
        fprintf (stream, "%u", value);
    }

    void write_data (FILE* stream, const long &value)
    {
        fprintf (stream, "%ld", value);
    }

    void write_data (FILE* stream, const unsigned long &value)
    {
        fprintf (stream, "%lu", value);
    }

    void write_data (FILE* stream, const long long &value)
    {
        fprintf (stream, "%lld", value);
    }

    void write_data (FILE* stream, const unsigned long long &value)
    {
        fprintf (stream, "%llu", value);
    }

    void write_data (FILE* stream, const char &value)
    {
        fprintf (stream, "%c", value);
    }

    void write_data (FILE* stream, char* value)
    {
        fprintf (stream, "%s", value);
    }

    void write_data (FILE* stream, const float &value)
    {
        fprintf (stream, "%f", value);
    }

    void write_data (FILE* stream, const double &value)
    {
        fprintf (stream, "%lf", value);
    }

};
#endif
