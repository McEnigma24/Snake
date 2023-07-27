#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdio.h>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define SPACE 32

using std::cin;  using std::cout; using std::endl;
using std::vector;  using std::string; using std::ifstream;

typedef vector<int> vec1;
typedef vector<vec1> vec2;


char background = ' ';

char boundery_top = '-';
char boundery_down = '-';
char boundery_left = '[';
char boundery_right = ']';

char body = '0';
char head = '1';

char apple = '*';

size_t h = 25;
size_t l = 25;
int timee = 300;
int change = 25;
int FINAL_SPEED = 100;
int MEDIUM_SPEED = 150;
bool do_we_have_MEDIUM_SPEED = true;


char columns_separator = ' '; 
char rows_separator = '\n';



#define SKIP_LINE getline(FILE, buffer); //cout<<"buffer "<<buffer<<endl;
#define SKIP_NAME FILE >> buffer;  FILE >> buffer;
#define CHAR_SKIP getline(FILE, buffer);\
{\
index = 0;\
while (index < buffer.size())\
{\
    if (buffer[index] == '(') break;\
    index++;\
}\
}

#ifdef DEBUG_MODE
#define LOG(x) cout << #x <<" ->" << x << "<-" << endl;
#else
#define LOG(X)
#endif



namespace File_Operations
{
    inline void deleteExistingAndCreateNewDirectory(const string& path)
    {
        if (std::filesystem::is_directory(path)) std::filesystem::remove_all(path);
        std::filesystem::create_directory(path);
    }
    inline bool existsFile(const string& path) {
        ifstream f(path.c_str());
        return f.good();
    }
    inline bool existsDirectory(const string& path)
    {
        return std::filesystem::is_directory(path);
    }    
};
namespace fo = File_Operations;

void loadInfoFromFile()
{
    if (!fo::existsFile("properties.txt"))
    {
        system("cls");
        cout << "Could not load essenctial file: 'properties.txt'";
        Sleep(3000);
    }
    else
    {
        ifstream FILE("properties.txt");
        string buffer;        
        int index;

        // Size //
        {
            SKIP_LINE;

            SKIP_NAME;      FILE >> h;      LOG(h)
            SKIP_NAME;      FILE >> l;      LOG(l)
        }

        // Plane Characteristics //
        {
            SKIP_LINE;
            SKIP_LINE;

            CHAR_SKIP;      background = buffer[++index];           LOG(background)
            CHAR_SKIP;      boundery_top = buffer[++index];         LOG(boundery_top)
            CHAR_SKIP;      boundery_down = buffer[++index];        LOG(boundery_down)
            CHAR_SKIP;      boundery_left = buffer[++index];        LOG(boundery_left)
            CHAR_SKIP;      boundery_right = buffer[++index];       LOG(boundery_right)
            CHAR_SKIP;      body = buffer[++index];                 LOG(body)
            CHAR_SKIP;      head = buffer[++index];                 LOG(head)
            CHAR_SKIP;      apple = buffer[++index];                LOG(apple)
        }
        
        // Time between new move //
        {
            SKIP_LINE;

            SKIP_NAME;      FILE >> timee;                          LOG(timee)
            SKIP_NAME;      FILE >> change;                         LOG(change)
            SKIP_NAME;      FILE >> FINAL_SPEED;                    LOG(FINAL_SPEED)
            SKIP_NAME;      FILE >> MEDIUM_SPEED;                   LOG(MEDIUM_SPEED)
            SKIP_NAME;      FILE >> do_we_have_MEDIUM_SPEED;        LOG(do_we_have_MEDIUM_SPEED)
        }

        // Separator //
        {
            SKIP_LINE;
            SKIP_LINE;            

            CHAR_SKIP;      columns_separator = buffer[++index];            LOG(columns_separator)
        }

        #ifdef DEBUG_MODE
            Sleep(3000);
        #endif // DEBUG_MODE
    }
}

class Snake
{
    struct move
    {
        int i, j;
        move(int ii, int jj)
            :i(ii), j(jj)
        {}

        move& operator=(const move& other)
        {
            this->i = other.i;
            this->j = other.j;

            return *this;
        }
    };
    struct position
    {
        int i, j;
        position(int ii, int jj)
            :i(ii), j(jj)
        {}
        position(const position& other)
            :i(other.i), j(other.j)
        {}
        position(int max_i, int max_j, bool random)
        {
            i = (rand() % (max_i - 2)) + 1;
            j = (rand() % (max_j - 2)) + 1;
        }

        void randomize(int max_i, int max_j)
        {
            i = (rand() % (max_i - 2)) + 1;
            j = (rand() % (max_j - 2)) + 1;
        }

        bool operator==(const position& other)
        {
            return ((this->i == other.i) && ((this->j == other.j)));
        }
        bool operator!=(const position& other)
        {
            return !this->operator==(other);
        }

        position& operator+=(const move& m)
        {
            this->i += m.i;
            this->j += m.j;

            return *this;
        }
    };
    struct snake_fragment
    {
        position my_position;
        position old_position;

        snake_fragment(int i, int j, int ii, int jj)
            :my_position(i, j), old_position(ii, jj)
        {
        }
        snake_fragment(position first, position second)
            :my_position(first), old_position(second)
        {
        }

        /*
        snake_fragment operator[](int index)
        {
            for each (auto& fragment in snake_fragment)
            {

            }
        }
        */
    };

    bool sepereate_columns;
    bool sepereate_rows;    
    int time_in_between;
    int how_much_the_time_changes;


    vec2 grid;  size_t height;  size_t length;
    position pos_apple;
    move move_current;
    move move_user_input;
    move move_before_pause;
    bool pause;
    bool first_division;
    vector<snake_fragment> vec_snake_segments;

public:

    // Set up
    Snake(size_t h, size_t l, int tib, int change_tib, bool sep_columns = true, bool sep_rows = true)
        :sepereate_columns(sep_columns), sepereate_rows(sep_rows), time_in_between(tib), how_much_the_time_changes(change_tib),
        grid(h + 1, vec1(l + 1, background)), height(h), length(l),
        pos_apple(-1, -1), move_current(0, 1), move_user_input(0, 1), move_before_pause(0, 1), pause(false), first_division(true)        
    {
        vec_snake_segments.reserve(100);
        vec_snake_segments.emplace_back(h / 2, l / 2, -1, -1); // first is head, second is NULL -1, -1

        markBounderies(grid);
        grid[h / 2][l / 2] = head;
    }
    void markBounderies(vec2& v)
    {
        // up boudery
        for (size_t j = 0; j < length; j++) v[0][j] = boundery_top;
        // low boudery
        for (size_t j = 0; j < length; j++) v[height - 1][j] = boundery_down;



        // left boudery
        for (size_t i = 0; i < height; i++) v[i][0] = boundery_left;
        // rigth boudery
        for (size_t i = 0; i < height; i++) v[i][length - 1] = boundery_right;
    }


    // Events
    int getUserInput()
    {
        if (_kbhit())
            return _getch();
        else
            return -1;
    }
    void gatheringUserInputForSomeTime(int miliseconds)
    {
        auto start = std::chrono::system_clock::now();
        auto end = std::chrono::system_clock::now();
        int key_pressed = 0;


        while ((std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() < miliseconds))
        {
            end = std::chrono::system_clock::now();

            key_pressed = getUserInput();

            if (key_pressed == -1) continue;
            else
            {
                switch (key_pressed)
                {
                case KEY_UP:
                {
                    if (!(move_current.i == 1 && move_current.j == 0))
                    {
                        move_user_input.i = -1;
                        move_user_input.j = 0;
                    }
                    break;
                }

                case KEY_DOWN:
                {
                    if (!(move_current.i == -1 && move_current.j == 0))
                    {
                        move_user_input.i = 1;
                        move_user_input.j = 0;
                    }
                    break;
                }

                case KEY_LEFT:
                {
                    if (!(move_current.i == 0 && move_current.j == 1))
                    {
                        move_user_input.i = 0;
                        move_user_input.j = -1;
                    }
                    break;
                }

                case KEY_RIGHT:
                {
                    if (!(move_current.i == 0 && move_current.j == -1))
                    {
                        move_user_input.i = 0;
                        move_user_input.j = 1;
                    }
                    break;
                }

                case SPACE:
                {
                    pause = !pause;

                    if (pause)
                    {
                        move_before_pause.i = move_current.i;
                        move_before_pause.j = move_current.j;

                        move_current.i = 0;
                        move_current.j = 0;
                    }
                    else
                    {
                        move_current.i = move_before_pause.i;
                        move_current.j = move_before_pause.j;
                    }


                    break;
                }

                default:
                    break;
                }
            }
        }
    }

    void checkWhereSnakeWillMove(bool& game_running, bool& apple_eaten)
    {
        position future_snake_position(vec_snake_segments[0].my_position);
        future_snake_position += move_current; /////////////////////////////////////////////////////////////////////////////

        // boundery hit
        if (future_snake_position.i == 0 || future_snake_position.j == 0 ||
            future_snake_position.i == height - 1 || future_snake_position.j == length - 1)
        {
            cout << "boundery hit" << endl;
            game_running = false;
            return;
        }

        // self hit
        if (grid[future_snake_position.i][future_snake_position.j] == body)
        {
            cout << "self hit" << endl;
            game_running = false;
            return;
        }

        // apple eaten
        if (grid[future_snake_position.i][future_snake_position.j] == apple)
        {
            apple_eaten = true;
            return;
        }
    }

    position newApplePosition()
    {
        position new_apple_pos(height, length, 1);

        while (grid[new_apple_pos.i][new_apple_pos.j] != background)
            new_apple_pos.randomize(height, length);

        return new_apple_pos;
    }
    void updateGridAccordingly(bool& apple_eaten)
    {
        size_t snake_size = vec_snake_segments.size();
        size_t snake_head = 0;
        size_t snake_tail = snake_size - 1;


        // Head
        vec_snake_segments[snake_head].old_position = vec_snake_segments[snake_head].my_position;
        vec_snake_segments[snake_head].my_position.i += move_current.i;
        vec_snake_segments[snake_head].my_position.j += move_current.j;

        // Body
        for (size_t i = 1; i <= snake_tail; i++)
        {
            vec_snake_segments[i].old_position = vec_snake_segments[i].my_position;
            vec_snake_segments[i].my_position = vec_snake_segments[i - 1].old_position;
        }

        // w grid
        // zmiana starej pozycji head na 'body'
        // zapalenie nowej pozycji head na 'head'

        grid[vec_snake_segments[snake_head].my_position.i][vec_snake_segments[snake_head].my_position.j] = head;
        if (snake_size > 1) grid[vec_snake_segments[snake_head].old_position.i][vec_snake_segments[snake_head].old_position.j] = body;
        // dla nowego elementu nie trzeba zapalaæ, bo ju¿ jest z wczeœniejszego


        // Adding size to snake
        if (apple_eaten)
        {
            // dodawanie nowego elementu            
            vec_snake_segments.emplace_back(vec_snake_segments[snake_tail].old_position, position(-1, -1));
            grid[vec_snake_segments[snake_tail].old_position.i][vec_snake_segments[snake_tail].old_position.j] = body;

            pos_apple = newApplePosition();
            grid[pos_apple.i][pos_apple.j] = apple;

            apple_eaten = false;

            if (first_division && do_we_have_MEDIUM_SPEED && time_in_between < MEDIUM_SPEED)
            {
                how_much_the_time_changes /= 2;
                first_division = false;
            }
            if (time_in_between < FINAL_SPEED) how_much_the_time_changes = 0;

            time_in_between -= how_much_the_time_changes;
        }
        else
        {
            // zgaszenie w grid old_position ostatniego elementu

            grid[vec_snake_segments[snake_tail].old_position.i][vec_snake_segments[snake_tail].old_position.j] = background;
        }
    }

    // Refreshing
    void visualizeMap()
    {
        string buffer;

        for (size_t i = 0; i < height; i++)
        {
            for (size_t j = 0; j < length; j++)
            {
                buffer += (char)grid[i][j];
                buffer += (char)columns_separator;
            }

            buffer += '\n';
        }

        cout << buffer << endl;
    }

    void play()
    {
        bool game_running = true;
        bool apple_eaten = false;
        position first_apple_position = newApplePosition();
        grid[first_apple_position.i][first_apple_position.j] = apple;


        cout << "Let's play" << endl; Sleep(1000);

        while (game_running)
        {
            system("cls");

            // Events            
            if (!pause)
            {
                move_current = move_user_input;

                checkWhereSnakeWillMove(game_running, apple_eaten);
                if (game_running == false)
                {
                    if (time_in_between < FINAL_SPEED) cout << "FINAL SPEED" << '\n';

                    visualizeMap();

                    cout << "speed " << time_in_between << "   elements " << vec_snake_segments.size() << '\n';
                    Sleep(3000);
                    return;
                }

                updateGridAccordingly(apple_eaten);
            }

            else cout << "GAME PAUSED" << '\n';
            if (time_in_between < FINAL_SPEED) cout << "FINAL SPEED" << '\n';
            visualizeMap();

            cout << "speed " << time_in_between << "   elements " << vec_snake_segments.size() << '\n';
            //cout << "accepted move ( " << move_current.i << ", " << move_current.j << ")" << '\n';

#ifdef DEBUG_MODE

            int i = 1;
            for (auto& v : vec_snake_segments)
            {
                cout << endl << i << ". my position: (";
                cout << v.my_position.i << " " << v.my_position.j << ")" << endl;
            }

#endif

            gatheringUserInputForSomeTime(time_in_between);
        }
    }
};



int main()
{
    srand((unsigned int)time(NULL));

    loadInfoFromFile();

    Snake game(h, l, timee, change);
    game.play();

    return 0;
}