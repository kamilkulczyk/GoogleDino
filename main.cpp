#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <conio.h>
#include <windows.h>
#include <cwchar>

using namespace std;

const int screen_width=25,screen_height=25;
const char n='.';
char screen[screen_height][screen_width];
bool przegrana=false;


void clearScreen() {
    for(int y=0;y<screen_height;y++) {
        for(int x=0;x<screen_width;x++) {
            screen[x][y]=n;
        }
    }
}

class Shape { //abstract class
    public:
        int x,y; //left top position
        char ch; //char to print
        int delta=0;
        int width,height; //hitbox
        virtual void drawObject() {}
        virtual void animateObject(bool jump, bool squat) {}
        bool isAnimated() {return is_animated;}
    protected:
        bool is_animated=false;
};

class Rect : public Shape {
    public:
        Rect(int x,int y, int width, int height) {
            this->x=x;
            this->y=y;
            this->width=width;
            this->height=height;
            ch=177;
        }
        virtual void drawObject () {
            for(int yc=y;yc<height+y;yc++) {
                for(int xc=x;xc<width+x;xc++) {screen[yc][xc]=ch;}
            }
        }
};

class Dinozaur : public Shape {
    public:
        Dinozaur(int x, int y) {
            this->x=x;
            this->y=y;
            height=4;
            width=5;
            ch=177;
        }
        virtual void drawObject() {
            screen[y][x+3]=ch;
            screen[y+1][x]=screen[y+1][x+2]=screen[y+1][x+3]=screen[y+1][x+4]=ch;
            screen[y+2][x+1]=screen[y+2][x+2]=screen[y+2][x+3]=ch;
            screen[y+3][x+2]=ch;
        }
};

class StickMan : public Shape {
    public:
        StickMan(int x, int y) {
            this->x=x;
            this->y=y;
            height=4;
            width=3;
            ch=177;
        }
        virtual void drawObject() {
            screen[y][x+1]=ch;
            screen[y+1][x]=screen[y+1][x+1]=screen[y+1][x+2]=ch;
            screen[y+2][x+1]=ch;
            screen[y+3][x]=screen[y+3][x+2]=ch;
        }
};

class StickMan2 : public Shape {
    public:
        StickMan2(int x, int y) {
            this->x=x;
            this->y=y;
            height=8;
            width=3;
            ch=177;
            is_animated=true;
        }
        char graphic_idle[8][5]={
        {n,n,ch,n,n},
        {n,n,ch,n,n},
        {n,ch,ch,ch,n},
        {ch,n,ch,n,ch},
        {n,n,ch,n,n},
        {n,ch,n,ch,n},
        {n,ch,n,ch,n},
        {n,ch,n,ch,n}
        };
        char graphic_squat[8][5]={
        {n,n,n,n,n},
        {n,n,n,n,n},
        {n,n,n,n,n},
        {ch,ch,ch,ch,ch},
        {n,n,ch,n,n},
        {n,ch,n,ch,n},
        {n,ch,n,ch,n},
        {n,ch,n,ch,n}
        };
        char graphic_squat2[8][5]={
        {n,n,n,n,n},
        {n,n,n,n,n},
        {n,n,n,n,n},
        {n,n,n,n,n},
        {ch,ch,ch,ch,ch},
        {n,n,ch,n,n},
        {n,ch,n,ch,n},
        {n,ch,n,ch,n},
        };
        char graphic_jump[8][5]={
        {n,n,ch,n,n},
        {ch,n,ch,n,ch},
        {n,ch,ch,ch,n},
        {n,n,ch,n,n},
        {n,n,ch,n,n},
        {n,ch,n,ch,n},
        {n,ch,n,ch,n},
        {n,ch,n,ch,n}
        };

        virtual void animateObject(bool is_falling, bool squat) {

            if(is_falling) {
                for(int i=0;i<8;i++) {
                    for(int j=0;j<5;j++) {
                        if(graphic_jump[i][j]!=n) screen[y+i][x+j]=ch;
                    }
                }
            } else if(squat) {
                for(int i=0;i<8;i++) {
                    for(int j=0;j<5;j++) {
                        if(graphic_squat[i][j]!=n) screen[y+i][x+j]=ch;
                    }
                }
            } else {
                for(int i=0;i<8;i++) {
                    for(int j=0;j<5;j++) {
                        if(graphic_idle[i][j]!=n) screen[y+i][x+j]=ch;
                    }
                }
            }
        }
};

class Player {
    public:
        int width, height;
        int speed1=8;
        int gravity=1;
        bool can_jump=true;
        bool is_falling=false;
        bool is_squating=false;
        bool can_right=true;
        bool can_left=true;

        Shape *shape=new Shape;
        Player(Shape* sh) {
            shape=sh;
            width=shape->x;
            height=shape->y;
            shape->ch=178;
        }
        ~Player() {
            shape=nullptr;
            delete shape;
        }
        void drawObject() {
            if(shape->isAnimated()) shape->animateObject(is_falling, is_squating);
            else {shape->drawObject();}
        }
};

class Obstacle {
   public:
        int width, height;
        int speed1=4;
        Shape *shape=new Shape;
        Obstacle(Shape* sh) {
            shape=sh;
            width=sh->x;
            height=sh->y;
            shape->ch=176;
        }
        ~Obstacle() {
            shape=nullptr;
            delete shape;
        }
        void drawObject() {
            shape->drawObject();
        }
};

void displayScreen() {
    string buffer;
    cout<<"Use WSAD";
    for(int i=0;i<screen_height+1;i++)
        cout<<"\e[F";
        //cout << "\x1b[A";
    for(int y=0;y<screen_height;y++) {
                for(int x=0;x<screen_width;x++) {
                    buffer+=screen[y][x];
                } buffer+='\n';
    }
    cout<<buffer;
}

bool collisionFloor(Shape shape) {
    for(int i=0;i<shape.width;i++) {
        if(screen[shape.y+shape.height][shape.x+i]!=n)return false;
    } return true;
}
bool collisionRoof(Shape shape) {
    for(int j=shape.delta;j>=0;j--) {
        for(int i=0;i<shape.width;i++) {
            if(screen[shape.y+j-1][shape.x+i]!=n) return true;
        }
    } return false;
}
bool collisionRight(Shape shape) {
    for(int i=shape.height-1;i>=shape.delta;i--) {
        if(screen[shape.y+i][shape.x+shape.width]!=n||shape.x+shape.width==screen_width) return true;
    } return false;
}
bool collisionLeft(Shape shape) {
    for(int i=shape.height-1;i>=shape.delta;i--) {
        if(screen[shape.y+i][shape.x-1]!=n||shape.x==0) return true;
    } return false;
}

int main()
{
    int frame=5;
    {//unlock ANSI
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
           DWORD mode = 0;
           if (GetConsoleMode(hConsole, &mode)) {
              mode |=0x0004;
              SetConsoleMode(hConsole, mode);
           }
        }
    }

    { //console window and font settings
      HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD a;
        a.X = 90;
        a.Y = 300;
        SMALL_RECT rect;
        rect.Top = 0;
        rect.Left = 0;
        rect.Bottom = 25;
        rect.Right = 25;
        SetConsoleScreenBufferSize(handle, a);
        SetConsoleWindowInfo(handle, 1, &rect);
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = 30;                   // Width of each character in the font
        cfi.dwFontSize.Y = 30;                  // Height
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
    }

    Rect r01(4,18,5,1);
    Rect r02(10,19,3,1);
    StickMan2 p1(5,5);

    Player player(&p1);

    Rect r3(16,23,2,2);
    Rect r4(8,17,2,2);
    Obstacle o1(&r3);
    Obstacle o2(&r4);

    bool end_condition=true;

    //start screen
    cout<<"CHROME DINO LIKE GAME\n\n\n\n\n\n";
    cout<<"press any key to start";
    getchar();

    clearScreen();
    while(!(GetKeyState(VK_ESCAPE) & 0x8000)&&end_condition) {//Finish when "Esc" is pressed
            if(frame>20) frame=0;
        if(GetKeyState('W') & 0x8000/*Check if high-order bit is set (1 << 15)*/) {
            if(player.can_jump) {
                player.gravity=1;
                if(frame>0&&frame%5==0) player.shape->y-=player.speed1;
            }
        } if(GetKeyState('S') & 0x8000/*Check if high-order bit is set (1 << 15)*/) {
            player.is_squating=true;
            //player.gravity=3;
            player.shape->delta=3;
        } else if(!collisionRoof(*player.shape)) {player.is_squating=false;player.shape->delta=0;}

        //clear screen from objects
        clearScreen();

        //draw things
        //r01.drawObject();
        //r02.drawObject();

        if(o1.shape->x<=0) o1.shape->x=24;
        if(o2.shape->x<=0) o2.shape->x=24;
        if(frame>0&&frame%4==0) {o1.shape->x--;o2.shape->x--;}
        o1.drawObject();
        o2.drawObject();

        if(collisionRoof(*player.shape)) player.can_jump=false;
        else player.can_jump=true;
        player.can_right=true;
        player.can_left=true;
        player.is_falling=false;

        if(collisionFloor(*player.shape)) {
            if(frame>0&&frame%4==0) player.shape->y+=player.gravity;
            player.can_jump=false;
            player.is_falling=true;
        } if(collisionRight(*player.shape)) {
            player.can_right=false;
            cout<<"You lose!";Sleep(1000);
            end_condition=false;
        } if(collisionLeft(*player.shape)) {
            player.can_left=false;
        }

        player.drawObject();

        //render things
        displayScreen();
        Sleep(10);frame++;

    }
    cout<<"Thanks for playing!";Sleep(2000);
    return 0;
}
