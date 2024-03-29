#include "bin-tree.hpp"
#include <cassert>
#include <unistd.h>



static const int BUFSIZE = 512;
static const int INPUTSIZE = 256;
static const char* TREE_INPUT = "tree-base.txt";
static const char* TREE_OUTPUT = "tree-base.txt";


class Akinator_tree : public Tree_t<char*>
{
    char symb = 0;
    int read_count = 0;
    char buf[BUFSIZE] = "";
    bool YES_flag = false;

    void free_tree (Node_t *tree) override
    {
        if (tree)
        {
            free_tree (tree -> left );
            free_tree (tree -> right);
            free (tree -> data);
            delete (tree);
        }
    }

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

    ~Akinator_tree() override
    {
        free_tree (head);
    }

    friend void go_lower_and_ask (Akinator_tree &tree, Akinator_tree::Node_t* &node, char* voice_command, char* userInput);
    friend void add_character (Akinator_tree &tree, Akinator_tree::Node_t* &node, char* userInput);
    friend void comporation_mode (Akinator_tree &tree, char* userInput);
};

void print_and_say (const char* speech)
{
    printf ("%s", speech);
    fflush (stdout);
    char voice [BUFSIZE] = {};
    strcpy (voice, "echo \"");
    strcat (voice, speech);
    strcat (voice, "\" | festival --tts --language russian");
    system (voice);
}


bool check_answer (char* userInput)
{
    print_and_say ("\nЯ угадал?\n");
    {
        scanf ("%s", userInput);

        if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
            return true;
        else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
            return false;
        else
            printf ("Неправильный ответ, попробуй ещё\n");
            system ("echo \"Неправильный ответ, попробуй ещё\" | festival --tts --language russian");
    }
}

void add_character (Akinator_tree &tree, Akinator_tree::Node_t* &node, char* userInput)
{
    int read_count = 0;
    printf (":(");
    print_and_say ("\nХорошо, кто это был?\n");
    scanf (" %[^\n]%n", userInput, &read_count );

    tree.make_right (node, nullptr);
    node -> right -> data = (char*) calloc ( read_count + 1, sizeof (char) );
    strcpy (node -> right -> data, userInput);

    tree.make_left (node, nullptr);
    node -> left -> data = node -> data;

    print_and_say ("И чем же ваш персонаж отличается от моего?\n");
    scanf (" %[^\n]%n", userInput, &read_count );
    node -> data = (char*)calloc (read_count + 1, sizeof (char) );
    strcpy (node -> data, userInput);
}


void go_lower_and_ask (Akinator_tree &tree, Akinator_tree::Node_t* &node, char* voice_command, char* userInput)
{
    if ( node -> right && node -> left )
    {
        printf ("\033[1;36m");
        print_and_say (node -> data);
        printf ("\033[1;36m");
        printf ("\033[0m");
        printf ("?\n");
        system (voice_command);
        while (1)
        {
            scanf ("%s", userInput);

            if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
            {
                go_lower_and_ask (tree, node -> right, voice_command, userInput);
                break;
            }
            else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
            {
                go_lower_and_ask (tree, node -> left, voice_command, userInput);
                break;
            }
            else
            {
                print_and_say ("Неправильный ответ, попробуй ещё\n");
            }
        }
    }
    else
    {

        printf ("\033[1;32m");
        print_and_say (node -> data);
        printf ("\033[0m");


        if ( ! check_answer(userInput) )
        {
            add_character (tree, node, userInput);
            #ifndef FREE_FROM_BASE_TREE
                tree.write_tree (TREE_OUTPUT);
            #endif
        }
    }
}

void find_mode (Akinator_tree &tree, char* userInput)
{
    char voice_command[BUFSIZE] = {};
    printf ("Здравствуй, мой друг. Ты точно готов поразиться моей гениальностью?\n");
    system ("echo \"Предупреждаю, моя гениальность шокирует. Ты готов офигеть?\" | festival --tts --language russian");

    while (1)
    {
        scanf ("%s", userInput);
        if ( ! strcmp ("да", userInput) || ! strcmp ("yes", userInput) )
        {
            go_lower_and_ask (tree, tree.head, voice_command, userInput);
            break;
        }
        else if ( ! strcmp ("нет", userInput) || ! strcmp ("no", userInput) )
        {
            return;
        }
        else
        {
            print_and_say ("Неправильный ответ, попробуй ещё");
            printf ("\n");
        }
    }
}

Akinator_tree::Node_t* write_equals (Akinator_tree::Node_t *node, char* char1, char* char2, long &depth, char* voice_command)
{
    Akinator_tree::Node_t* last_node = nullptr;
    depth++;
    if (char1 [depth] == char2 [depth])
    {

        if ( char1 [depth] == 0 )
        {
            strcat (voice_command, " Не ");
        }
        else
        {
            strcat (voice_command, " ");
        }
        strcat (voice_command, node -> data);
        strcat (voice_command, ",");

    }
    else
        return node;
    if (char1 [depth] == char2 [depth])
        if ( char1 [depth] == 0 )
            last_node = write_equals (node -> left , char1, char2, depth, voice_command);
        else
            last_node = write_equals (node -> right, char1, char2, depth, voice_command);
    else
    return last_node;
}

void write_unequals (Akinator_tree::Node_t *node, char* charact, long depth, char* voice_command)
{
    if ( ! node -> left && ! node -> right )
        return;

    if (charact [depth] == 0)
    {
        strcat (voice_command, " Не ");
    }
    else
    {
        strcat (voice_command, " ");
    }
    
    strcat (voice_command, node -> data);
    strcat (voice_command, ",");
   
    if (charact [depth] == 0 )
    {
        write_unequals (node -> left, charact, depth+1, voice_command);
    }
    if (charact [depth] == 1 )
    {
        write_unequals (node -> right, charact, depth+1, voice_command);
    }
}

void write_comporation (Akinator_tree::Node_t *node, char* name1, char* name2, char* char1, char* char2, long &depth, char* voice_command)
{
    strcpy (voice_command, name1);
    strcat (voice_command, " и ");
    strcat (voice_command, name2);
    strcat (voice_command, " похожи тем что они");

    Akinator_tree::Node_t* first_unequal = write_equals (node, char1, char2, depth, voice_command);
    
    printf ("\033[1;32m");
    print_and_say (voice_command);
    printf ("\033[0m");

    strcpy (voice_command, " но ");
    strcat (voice_command, name1);

    write_unequals (first_unequal, char1, depth, voice_command);

    strcat (voice_command, " а ");
    strcat (voice_command, name2);
    write_unequals (first_unequal, char2, depth, voice_command);

    printf ("\033[1;31m");
    print_and_say (voice_command);
    printf ("\033[0m");
}

void comporation_mode (Akinator_tree &tree, char* userInput)
{
    char voice_command [BUFSIZE] = {};

    char character1[256] ="";
    long depth = 0;
    char* characteristics1 = (char*) calloc (tree.node_counter, sizeof (char));
    char* characteristics2 = (char*) calloc (tree.node_counter, sizeof (char));

    print_and_say ("Кого вы хотите сравнить?\n");
    scanf (" %[^\n]", userInput);
    if ( ! tree.deep_search (tree.head, userInput, characteristics1, depth) )
    {
        print_and_say ("Такого персонажа нет. Если хочешь просмотреть персонажей, попроси меня вывести дерево в меню");
        return;
    }
    strcpy (character1, userInput);
    print_and_say ("С кем сравниваем?\n");
    scanf (" %[^\n]", userInput);
    if ( ! strcmp (character1, userInput) )
    {
        printf ("Так это один и тот же персонаж!");
        system ("echo \"Ты издеваешься?\" | festival --tts --language russian");
        return;
    }

    depth = 0;
    if ( ! tree.deep_search (tree.head, userInput, characteristics2, depth) )
    {
        print_and_say ("Такого персонажа нет\n");
        return;
    }
    depth = 0;
    write_comporation (tree.head, character1, userInput, characteristics1, characteristics2, depth, voice_command);
}

void menus (Akinator_tree &tree)
{
    char user_input[INPUTSIZE] = {};
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
        printf ("(─‿‿─)\n\n\n//Акинатор приехал с Казахстана, поэтому знает мало слов на нашем языке.\n"
                "Чтобы Акинатор наверняка понял тебя, старайся отвечать да (yes) или нет (no)\n\n");
        system ("echo \"Выберите режим\" | festival --tts --language russian");
        while (1)
        {
            scanf  (" %s", user_input);
            if ( user_input [0] == '1' )
            {
                system("clear");
                find_mode (tree, user_input);
                break;
            }
            else if ( user_input [0] == '2' )
            {
                system("clear");
                comporation_mode (tree, user_input);
                $p;
                $p;
                break;
            }
            else if ( user_input [0] == '3' )
            {
                system ("clear");
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
                print_and_say ("Не понимаю, скажи ещё раз");
                printf ("\n");
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
