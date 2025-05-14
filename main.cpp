#include <iostream>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include "const.h"
#include "utils.h"

/**score**/

//highscore
void SaveHighScore(int score) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

//highscore on screen
int LoadHighScore() {
    std::ifstream file("highscore.txt");
    int highscore = 0;
    if (file.is_open()) {
        file >> highscore;
        file.close();
    }
    return highscore;
}

//Init ttf
bool InitTTF() {
    if (TTF_Init() < 0) {
        std::cout << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }
    return true;
}

TTF_Font* LoadFont(const std::string& path, int size) {
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    return font;
}


SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;


/**Intro**/
class Intro
{
public:
    Intro();
    ~Intro ();

    void free();

    bool loadFromfile(std::string a);

    void setcolor (Uint8 r, Uint8 g, Uint8 b);

    void setBlender (SDL_BlendMode blending);

    void setalpha (Uint8 alpha);

    void render (int x, int y, SDL_Rect* clip = NULL);

    int getWidth();
    int getHeight();
private:
    SDL_Texture* mTexure;
    int mWidth;
    int mHeight;
};

Intro gIntroscreen;
Intro gPressEnter2start;
SDL_Rect gPressClip[2];

Intro::Intro()
{
    mTexure = NULL;
    mWidth = 0;
    mHeight = 0;
}

Intro::~Intro()
{
    free();
}

void Intro::free()
{
    if (mTexure != NULL)
    {
        SDL_DestroyTexture (mTexure);
        mTexure = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

bool Intro::loadFromfile(std::string a)
{
    free();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedsurface = IMG_Load(a.c_str());
    if (loadedsurface == NULL)
    {
        std::cout<<"K tim thay link: "<<a.c_str()<<IMG_GetError();
    }
    else
    {
        SDL_SetColorKey (loadedsurface,SDL_TRUE,SDL_MapRGB(loadedsurface->format,0,0xFF,0xFF));
        newTexture = SDL_CreateTextureFromSurface (gRenderer,loadedsurface);
        if (newTexture == NULL)
        {
            std::cout<<"K tao dc texture "<<SDL_GetError();
        }
        else
        {
            mWidth = loadedsurface->w;
            mHeight = loadedsurface->h;
        }
    }
    SDL_FreeSurface(loadedsurface);
    mTexure = newTexture;
    return mTexure !=NULL;
}

void Intro::setalpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod (mTexure, alpha);
}

void Intro::setBlender(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode (mTexure,blending);
}

void Intro::setcolor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(mTexure, r, g, b);
}

int Intro::getHeight()
{
    return mHeight;
}

int Intro::getWidth()
{
    return mWidth;
}

void Intro::render(int x, int y, SDL_Rect* clip)
{
    SDL_Rect renderQuad = {x,y,SCREEN_WIDTH,SCREEN_HEIGHT};
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
        SDL_RenderCopy (gRenderer,mTexure,clip,&renderQuad);
    }
    else
    {
        SDL_RenderCopy (gRenderer,mTexure,NULL,&renderQuad);
    }
}


/**Gameplay**/
class LGameplay {
public:
    LGameplay();
    ~LGameplay();
    bool loadFromFile(std::string path);
    void free();
    void render(int x=0, int y=0, SDL_Rect* clip = NULL, int shrink = 0, double angle =0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    int getWidth();
    int getHeight();
    int planeHP = 1000;

private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
};

SDL_Rect gPlaneClips[FLYING_FRAMES];
LGameplay gPlane; // plane texture
LGameplay gBackGround; // background texture
LGameplay gBackGround2;

LGameplay::LGameplay() {
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LGameplay::~LGameplay() {
    free();
}

bool LGameplay::loadFromFile(std::string path) {
    free();
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        std::cout << "Không thể load ảnh: " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (mTexture == NULL) {
        std::cout << "Không thể tạo texture: " << SDL_GetError() << std::endl;
        return false;
    }

    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);
    return true;
}

void LGameplay::free() {
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LGameplay::render(int x, int y, SDL_Rect* clip,int shrink , double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };
    if (clip != NULL && shrink == 0) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
        SDL_Rect shrinkplane = {x,y,PLANE_WIDTH,PLANE_HEIGHT};
        SDL_RenderCopyEx (gRenderer, mTexture, clip, &shrinkplane, angle, center, flip);
    }
    else
    {
        SDL_RenderCopyEx (gRenderer,mTexture,clip,&renderQuad,angle,center,flip);
    }
}

int LGameplay::getWidth() {
    return mWidth;
}

int LGameplay::getHeight() {
    return mHeight;
}


/**Bullet**/
class Bullet
{
public:
    Bullet(int x, int y);
    void movement();
    void render ();
    bool isOffscreen() const;
    SDL_Rect& getBox();
    int damage = 100;

private:
    SDL_Rect mBox;
    int speed;
};

Bullet::Bullet(int x, int y)
{
    mBox = {x+ PLANE_WIDTH/2 -5, y , 10, 20};
    speed = 7;
}

void Bullet::movement()
{
    mBox.y -= speed;
}

void Bullet::render()
{
    SDL_SetRenderDrawColor (gRenderer,0xFF,0,0,0);
    SDL_Rect tip = {mBox.x +2,mBox.y, 6, 8};
    SDL_RenderFillRect(gRenderer, &tip);
    SDL_Rect body = {mBox.x, mBox.y + tip.h, mBox.w, mBox.h - tip.h};
    SDL_RenderFillRect(gRenderer, &body);
}
bool Bullet::isOffscreen() const
{
    return mBox.y + mBox.h <0;
}

SDL_Rect& Bullet::getBox()
{
    return mBox;
}

/**Secondary Bullets**/
class Bullet2
{
public:
    Bullet2(int x, int y);
    void movement();
    void render ();
    bool isOffscreen() const;
    SDL_Rect& getBox();
    int damage = 100;

private:
    SDL_Rect mBox;
    int speed;
};

Bullet2::Bullet2(int x, int y)
{
    mBox = {x+ PLANE_WIDTH/2 -5, y , 10, 20};
    speed = 7;
}

void Bullet2::movement()
{
    mBox.y -= speed;
}

void Bullet2::render()
{
    SDL_SetRenderDrawColor (gRenderer,3,132,252,0);
    SDL_Rect tip = {mBox.x +2,mBox.y, 6, 8};
    SDL_RenderFillRect(gRenderer, &tip);
    SDL_Rect body = {mBox.x, mBox.y + tip.h, mBox.w, mBox.h - tip.h};
    SDL_RenderFillRect(gRenderer, &body);
}
bool Bullet2::isOffscreen() const
{
    return mBox.y + mBox.h <0;
}

SDL_Rect& Bullet2::getBox()
{
    return mBox;
}


/**Upgraded Bullets**/
class Upgraded_Bullet
{
private:
    SDL_Rect mBox;
    int speed;
public:
    Upgraded_Bullet (int x, int y);
    void movement();
    void render();
};


/**Enemy's Bullet**/
class EnemyBullet
{
private:
    SDL_Rect mBox;
    int speed;

public:
    EnemyBullet (int x, int y, int enemysize);
    void movement ();
    void render ();
    bool isOffscreen () const;
    SDL_Rect& getBox();
    int damage = 80;
};

EnemyBullet::EnemyBullet(int x, int y, int enemysize)
{
    mBox = {x+ enemysize/2, y, 7, 15};
    speed = 4;
}

void EnemyBullet::movement()
{
    mBox.y += speed;
}

void EnemyBullet::render()
{
    SDL_SetRenderDrawColor (gRenderer, 173, 3, 252, 0);
    SDL_Rect body = {mBox.x, mBox.y, mBox.w, mBox.h};
    SDL_RenderFillRect(gRenderer, &body);
}

bool EnemyBullet::isOffscreen() const
{
    return mBox.y + mBox.h <0;
}

SDL_Rect& EnemyBullet::getBox()
{
    return mBox;
}



/**Enemy**/
class Enemy1
{
public:
    Enemy1 (int x, int y);
    void movement();
    void render ();
    bool isOffscreen() const;
    bool loadfromfile(std::string a);
    void free();
    SDL_Rect& getBox ();
    int HP = 300;

private:
    SDL_Texture* mTexture;
    SDL_Rect mBox;
    int speedX;
    int speedY;
};

Enemy1::Enemy1(int x, int y)
{
    mTexture = NULL;
    mBox = {x,y,ENEMY_WIDTH,ENEMY_HEIGHT};
    speedX = (rand() % 5) - 2 ;
    speedY = (rand() % 5) - 3 ;
    if (speedX == 0 && speedY == 0)
    {
        speedX ++;
        speedY ++;
    }
}

void Enemy1::movement()
{
    mBox.x += speedX;
    if (mBox.x <= 0 || mBox.x >= SCREEN_WIDTH)
    {
        speedX = -speedX;
    }

    mBox.y += speedY;
    if (mBox.y <= 0 || mBox.y >= SCREEN_HEIGHT / 4)
    {
        speedY = -speedY;
    }
}

void Enemy1::render()
{
    if (mTexture != NULL)
    {
        SDL_RenderCopy (gRenderer, mTexture, NULL, &mBox);
    }
    else
    {
        SDL_SetRenderDrawColor (gRenderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderFillRect (gRenderer, &mBox);
    }
}

bool Enemy1::isOffscreen() const
{
    return mBox.y >= SCREEN_HEIGHT;
}

bool Enemy1::loadfromfile(std::string a)
{
    free();

    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(a.c_str());
    if (loadedSurface == NULL)
    {
        std::cout<<"K tim thay file: "<<a.c_str()<<IMG_GetError();
    }
    else
    {
        SDL_SetColorKey (loadedSurface,SDL_TRUE,SDL_MapRGB(loadedSurface->format,0,0xFF,0xFF));
        newTexture = SDL_CreateTextureFromSurface (gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            std::cout<<"K tao dc texture "<<SDL_GetError();
        }
        SDL_FreeSurface(loadedSurface);
    }
    mTexture = newTexture;
    return mTexture != NULL;
}

void Enemy1::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture (mTexture);
        mTexture = NULL;
    }
}

SDL_Rect& Enemy1::getBox()
{
    return mBox;
}

/**Enemy2**/
class Enemy2
{
public:
    Enemy2 (int x, int y);
    void movement();
    bool loadfromfile (std::string a);
    void free ();
    void render ();
    bool isoffscreen () const;
    SDL_Rect& getBox();
    void entrance (int layer, int x);
    bool entranceDone = false;
    int HP = 400;

private:
    SDL_Texture* mTexture;
    SDL_Rect mBox;
    int speedX;
    int originX;
};


Enemy2::Enemy2(int x, int y)
{
    mTexture = NULL;
    mBox = {x,y,ENEMY2_WIDTH,ENEMY2_HEIGHT};
    speedX = 2;
    originX = x;
}

void Enemy2::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture (mTexture);
        mTexture = NULL;
        speedX = 0;
    }
}

void Enemy2::movement()
{
    mBox.x += speedX;
    if (mBox.x <= originX - 50 || mBox.x >= originX + 50)
    {
        speedX = -speedX;
    }
}

bool Enemy2::loadfromfile (std::string a)
{
    free();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(a.c_str());
    if (loadedSurface == NULL)
    {
        std::cout<<"K tim thay link: "<<a.c_str()<<IMG_GetError();
    }
    else
    {
        SDL_SetColorKey (loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format,0,0xFF,0xFF));
        newTexture = SDL_CreateTextureFromSurface (gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            std::cout<<"K tao dc newTexture "<<SDL_GetError();
        }
        SDL_FreeSurface(loadedSurface);
    }
    mTexture = newTexture;
    return mTexture != NULL;
}

void Enemy2::render ()
{
    if (mTexture != NULL)
    {
        SDL_RenderCopy (gRenderer, mTexture, NULL, &mBox);
    }
    else
    {
        SDL_SetRenderDrawColor (gRenderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderFillRect (gRenderer, &mBox);
    }
}

bool Enemy2::isoffscreen () const
{
    return (mBox.y >= SCREEN_HEIGHT || mBox.y <0);
}

SDL_Rect& Enemy2::getBox()
{
    return mBox;
}

void Enemy2::entrance(int layer, int x)
{
    if (layer == 1) {
        if (mBox.y < SCREEN_HEIGHT / 5) {
            mBox.y += x;
        } else {
            entranceDone = true;
        }
    }
    if (layer == 2) {
        if (mBox.y < SCREEN_HEIGHT / 5 - 90) {
            mBox.y += x;
        } else {
            entranceDone = true;
        }
    }
}

//Enemy 3
class Enemy3
{
private:
    SDL_Texture* mTexture;
    SDL_Rect mBox;
    int speedX;
    int speedY;

public:
    Enemy3 (int x, int y);
    void free();
    bool loadFromfile(std::string a);
    bool isOffscreen();
    void render();
    void movementType1 (std::string left_started);
    void movementType2 (std::string left_started);
    SDL_Rect& getBox();
    int getX();
    void resetposY(int i);
    int HP = 300;

};

Enemy3::Enemy3(int x, int y)
{
    mTexture = NULL;
    mBox = {x,y,ENEMY_WIDTH,ENEMY_HEIGHT};
    speedY = 3;
    speedX = 1;
}

void Enemy3::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture (mTexture);
        mTexture = NULL;
        speedX = 0;
        speedY = 0;
        mBox.h = 0;
        mBox.w = 0;
    }
}

bool Enemy3::loadFromfile (std::string a)
{
    free();
    SDL_Surface* loadedSurface = IMG_Load(a.c_str());
    if (loadedSurface == NULL)
    {
        std::cout<<"K tim thay link :"<<a.c_str()<<IMG_GetError();
    }
    else{
        SDL_SetColorKey (loadedSurface, SDL_TRUE, SDL_MapRGB (loadedSurface->format, 0, 0xFF, 0xFF));
        mTexture = SDL_CreateTextureFromSurface (gRenderer, loadedSurface);
        if (mTexture == NULL)
        {
            std::cout<<"K tao duoc Texture "<<SDL_GetError();
        }
        SDL_FreeSurface (loadedSurface);
    }
    return mTexture != NULL;
}

bool Enemy3::isOffscreen ()
{
    return (mBox.y > SCREEN_HEIGHT && mBox.x > SCREEN_WIDTH) || (mBox.y < 0 - ENEMY_HEIGHT  && mBox.x < 0 - ENEMY_WIDTH);
}

void Enemy3::render()
{
    if (mTexture != NULL)
    {
        SDL_RenderCopy (gRenderer, mTexture, NULL, &mBox);
    }
    else
    {
        SDL_SetRenderDrawColor (gRenderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderFillRect (gRenderer, &mBox);
    }
}

void Enemy3::movementType1 (std::string left_started)
{
    if (left_started == "Left")
    {
        mBox.x += speedX;
    }
    else
    {
        mBox.x -= speedX;
    }

    mBox.y -= speedY;
    if (mBox.y >= SCREEN_HEIGHT / 4 + 60 || mBox.y <= SCREEN_HEIGHT/4 - 40)
    {
        speedY = -speedY;
    }
}

void Enemy3::movementType2 (std::string left_started)
{

    if (left_started == "Left")
    {
        mBox.y = 3*SCREEN_HEIGHT / 4 - 60;
        mBox.x -= 8*speedX;
    }
    else
    {
        mBox.y = 3*SCREEN_HEIGHT / 4;
        mBox.x += 8*speedX;
    }
}


SDL_Rect& Enemy3::getBox()
{
    return mBox;
}

int Enemy3::getX()
{
    return mBox.x;
}

void Enemy3::resetposY(int i)
{
    mBox.y = SCREEN_HEIGHT / 4 - 30 + 10*i;
}


/**Boss**/
class Boss
{
private:
    SDL_Texture* mTexture;
    SDL_Rect mBox;
    int speed;
public:
    Boss (int x, int y);
    void render(int x=0, int y=0, SDL_Rect* clip = NULL, double angle =0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void free();
    void movement(bool special);
    bool loadfromfile (std::string a);
    bool isOffscreen () const;
    SDL_Rect& getBox();
    int getX();
    int getY();
    int HP = 6000;
};

SDL_Rect BossClip [BOSS_FRAME];

Boss::Boss(int x, int y)
{
    mTexture = NULL;
    mBox = {x, y, BOSS_WIDTH, BOSS_HEIGHT};
    speed = 2;
}

void Boss::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x,y,BOSS_WIDTH,BOSS_HEIGHT};
    if (clip != NULL && mTexture != NULL)
    {
        SDL_RenderCopyEx (gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
    }
    else
    {
        SDL_SetRenderDrawColor (gRenderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderFillRect (gRenderer, &mBox);
    }
}

void Boss::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture (mTexture);
        mTexture = NULL;
        speed = 0;
        mBox.h = 0;
        mBox.w = 0;
    }
}

void Boss::movement(bool special)
{
    if (!special)
    {
        mBox.x += speed;
    }
    else
    {
        mBox.x += 0;
    }

    if (mBox.x >= SCREEN_WIDTH - BOSS_WIDTH || mBox.x <= 0)
    {
        speed = -speed;
    }
}

bool Boss::loadfromfile(std::string a)
{
    free();
    SDL_Surface* loadedSurface = IMG_Load(a.c_str());
    if (loadedSurface == NULL)
    {
        std::cout<<"K tim thay link :"<<a.c_str()<<IMG_GetError();
    }
    else{
        SDL_SetColorKey (loadedSurface, SDL_TRUE, SDL_MapRGB (loadedSurface->format, 0, 0xFF, 0xFF));
        mTexture = SDL_CreateTextureFromSurface (gRenderer, loadedSurface);
        if (mTexture == NULL)
        {
            std::cout<<"K tao duoc Texture "<<SDL_GetError();
        }
        SDL_FreeSurface (loadedSurface);
    }
    return mTexture != NULL;
}

bool Boss::isOffscreen() const
{
    return (mBox.y > SCREEN_HEIGHT && mBox.x > SCREEN_WIDTH) || (mBox.y < 0 - BOSS_HEIGHT  && mBox.x < 0 - BOSS_WIDTH);
}


SDL_Rect& Boss::getBox()
{
    return mBox;
}

int Boss::getY()
{
    return mBox.y;
}

int Boss::getX()
{
    return mBox.x;
}

/**Boss bullet**/
class BossBullet
{
private:
    SDL_Texture* mTexture;
    SDL_Rect mBox;
    int speed;
public:
    BossBullet (int x, int y);
    void free();
    bool loadfromfile (std::string a);
    bool isOffscreen ();
    void render();
    void movement();
    SDL_Rect& getBox();
    int damage = 200;
};

BossBullet::BossBullet(int x, int y)
{
    mTexture = NULL;
    mBox = {x,y,40,50};
    speed = 6;
}

void BossBullet::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture (mTexture);
        mTexture = NULL;
        speed = 0;
        mBox.h = 0;
        mBox.w = 0;
    }
}

bool BossBullet::loadfromfile(std::string a)
{
    free();
    SDL_Surface* loadedSurface = IMG_Load(a.c_str());
    if (loadedSurface == NULL)
    {
        std::cout<<"K tim thay link :"<<a.c_str()<<IMG_GetError();
    }
    else{
        SDL_SetColorKey (loadedSurface, SDL_TRUE, SDL_MapRGB (loadedSurface->format, 0, 0xFF, 0xFF));
        mTexture = SDL_CreateTextureFromSurface (gRenderer, loadedSurface);
        if (mTexture == NULL)
        {
            std::cout<<"K tao duoc Texture "<<SDL_GetError();
        }
        SDL_FreeSurface (loadedSurface);
    }
    return mTexture != NULL;
}

bool BossBullet::isOffscreen()
{
    return (mBox.y > SCREEN_HEIGHT && mBox.x > SCREEN_WIDTH) || (mBox.y < 0 - mBox.h  && mBox.x < 0 - mBox.w);
}

void BossBullet::render()
{
    if (mTexture != NULL)
    {
        SDL_RenderCopy (gRenderer, mTexture, NULL, &mBox);
    }
    else
    {
        SDL_SetRenderDrawColor (gRenderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderFillRect (gRenderer, &mBox);
    }
}

void BossBullet::movement()
{
    mBox.x +=0;
    mBox.y += speed;
}

SDL_Rect& BossBullet::getBox()
{
    return mBox;
}


/**Gameover**/
class Gameover
{
public:
    Gameover();
    ~Gameover();

    void free();

    bool loadFromfile(std::string a);

    void render (int x, int y, SDL_Rect* clip = NULL);

    int getWidth();
    int getHeight();
private:
    SDL_Texture* mTexure;
    int mWidth;
    int mHeight;
};

Gameover Gameoverscreen;
Gameover Press2restart;
SDL_Rect gRestartClip[2];

Gameover::Gameover()
{
    mTexure = NULL;
    mWidth = 0;
    mHeight = 0;
}

Gameover::~Gameover()
{
    free();
}

void Gameover::free()
{
    if (mTexure != NULL)
    {
        SDL_DestroyTexture (mTexure);
        mTexure = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

bool Gameover::loadFromfile(std::string a)
{
    free();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedsurface = IMG_Load(a.c_str());
    if (loadedsurface == NULL)
    {
        std::cout<<"K tim thay link: "<<a.c_str()<<IMG_GetError();
    }
    else
    {
        SDL_SetColorKey (loadedsurface,SDL_TRUE,SDL_MapRGB(loadedsurface->format,0,0xFF,0xFF));
        newTexture = SDL_CreateTextureFromSurface (gRenderer,loadedsurface);
        if (newTexture == NULL)
        {
            std::cout<<"K tao dc texture "<<SDL_GetError();
        }
        else
        {
            mWidth = loadedsurface->w;
            mHeight = loadedsurface->h;
        }
    }
    SDL_FreeSurface(loadedsurface);
    mTexure = newTexture;
    return mTexure !=NULL;
}


int Gameover::getHeight()
{
    return mHeight;
}

int Gameover::getWidth()
{
    return mWidth;
}

void Gameover::render(int x, int y, SDL_Rect* clip)
{
    SDL_Rect renderQuad = {x,y,SCREEN_WIDTH,SCREEN_HEIGHT};
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
        SDL_RenderCopy (gRenderer,mTexure,clip,&renderQuad);
    }
    else
    {
        SDL_RenderCopy (gRenderer,mTexure,NULL,&renderQuad);
    }
}


/**Main**/
bool init();
bool loadMedia();
void close();

bool init() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Không thể khởi tạo SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("Game Ban Ga", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        std::cout << "Không thể tạo cửa sổ: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL) {
        std::cout << "Không thể tạo renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "Không thể khởi tạo SDL_image: " << IMG_GetError() << std::endl;
        return false;
    }

    return success;
}

bool loadMedia() {
    bool success = true;
    if (!gPlane.loadFromFile("ABC/gameplane.png")) {
        std::cout << "Không thể load ảnh gameplane.png" << std::endl;
        success = false;
    } else {
        gPlaneClips[0] = { 233, 0, 233, 500 };
        gPlaneClips[1] = { 466, 0, 233, 500 };
        gPlaneClips[2] = { 699, 0, 233, 500 };
        gPlaneClips[3] = { 932, 0, 233, 500 };
        gPlaneClips[4] = { 1165, 0, 233, 500 };
    }

    if (! gBackGround.loadFromFile("ABC/backgroundgame.png"))
    {
        std::cout<<"K load dc anh background "<<SDL_GetError();
        success = false;
    }

    if (! gBackGround2.loadFromFile("ABC/backgroundgame2.png"))
    {
        std::cout<<"K load dc anh background2 "<<SDL_GetError();
        success = false;
    }

    if (! gIntroscreen.loadFromfile("ABC/Intropic.png"))
    {
        std::cout<<"K load dc anh Intro "<<SDL_GetError();
        success = false;
    }

    if (! gPressEnter2start.loadFromfile("ABC/pressEntertostart.png"))
    {
        std::cout<<"K load dc anh pressEntertostart "<<SDL_GetError();
        success = false;
    }
    else
    {
        gPressClip[0] = {0, 0, 300, 60};
        gPressClip[1] = {0, 60, 300, 60};
    }

    if (! Gameoverscreen.loadFromfile("ABC/GameOver.png"))
    {
        std::cout<<"K load dc anh Gameover "<<SDL_GetError();
        success = false;
    }

    if (! Press2restart.loadFromfile("ABC/Escape.png"))
    {
        std::cout<<"K load dc anh Exit "<<SDL_GetError();
        success = false;
    }
    else
    {
        gRestartClip[0] = {0, 0, 350, 60};
        gRestartClip[1] = {0, 60, 350, 60};
    }

    Boss boss1 (0,0);
    if (! boss1.loadfromfile("ABC/Boss.png"))
    {
        std::cout<<"K tim thay anh Boss "<<SDL_GetError();
        success = false;
    }
    else
    {
        BossClip[0] = {0,0,482,502};
        BossClip[1] = {482,0,482,502};
        BossClip[2] = {964,0,482,502};
        BossClip[3] = {1446,0,482,502};
    }
    return success;
}

void close() {
    //gameplay
    gPlane.free();

    //Intro
    gPressEnter2start.free();

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    Mix_CloseAudio();
}

int main(int argc, char* args[]) {
    if (!init()) {
        std::cout << "Khởi tạo thất bại!" << std::endl;
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cout << "SDL_mixer could not initialize! " << Mix_GetError() << std::endl;
    return 0;
    }

    if (!loadMedia()) {
        std::cout << "Tải media thất bại!" << std::endl;
        return 0;
    }

    //score
    if (!InitTTF()) return 0;
    TTF_Font* font = LoadFont("ABC/font1.ttf", 18);
    if (!font) return 0;

    int score = 0;
    int highscore = LoadHighScore();

    bool quit = false;
    SDL_Event e;

    //Intro
    std::string phase = "Intro";
    int pressFrame = 0;

    //Gameover
    int GameoverFrame = 0;

    //Gameplay
    int frame = 0;
    bool keyHeld [SDL_NUM_SCANCODES] = {false};

    // Vị trí nhân vật
    int Start_posY = SCREEN_HEIGHT - 5;
    int posX = (SCREEN_WIDTH - PLANE_WIDTH) / 2;
    int posY = (SCREEN_HEIGHT - PLANE_HEIGHT) - SCREEN_HEIGHT/6 ;
    // frame của plane
    int rightheldframe = 0;
    int leftheldframe = 0;
    int currentplaneFrame = 0;

    //cuộn mhinh
    int Scrollingoffset = 0;

    //Đạn
    std::vector<Bullet> bullets;
    int Bulletscooldown =0;

    //Enemy Bullet
    std::vector<EnemyBullet> enemybullet;
    std::vector<EnemyBullet> eb3Left;
    std::vector<EnemyBullet> eb3Right;
    int EB_cooldown = 0;

    //All enemy
    int currentType = 3  ;
    bool readyToChangeType = false;
    int typeChangeTimer = 0;
    bool currentWaveCleared = false;
    Uint32 waveClearTime = 0;
    bool waveWaiting = false;      // wave delay: 10s

    //Enemy1
    std::vector<Enemy1> enemies;
    int enemyspawntimer = 0;
    bool enemy1max = false;
    int Enemyadded =0;

    //Enemy2
    std::vector<Enemy2> enemies2;
    bool entrancePlayed = false;
    bool enemy2max = false;

    //Enemy3
    std::vector<Enemy3> enemies3Left;
    std::vector<Enemy3> enemies3Right;
    Uint32 timer = 0;
    bool enemy3max =false;
    int enemy3phase = 1;

    //Boss
    std::vector<Boss> boss;
    int counting = 1;
    int firetimer = 0;
    int currentBossFrame = 0;
    bool special = false;
    bool bossmax = false;
    Uint32 isspecial = 0;
    Uint32 specialshot = 0;
    int deadframe = 0;

    //BossBullet
    std::vector<BossBullet> bossBullet;
    int BB_cooldown = 0;

    //sound
    Mix_Chunk* shoot = NULL;
    Mix_Music* bgm = NULL;
    Mix_Chunk* gameoversound = NULL;

    //Màn hình vừa vào game
    bool quitIntro = false;
    bool quitentrance = false;
    bool quitRestart = false;

    while (!quit)
    {
        if (phase != "Gameover")
        {
            bgm = Mix_LoadMUS("ABC/background.ogg");
            if (!bgm) {
                std::cout << "Failed to load music: " << Mix_GetError() << std::endl;
            }
            Mix_VolumeMusic (MIX_MAX_VOLUME);
            Mix_PlayMusic(bgm, -1);
        }

        SDL_RenderClear(gRenderer);
        if (phase == "Intro")
        {

            while ( !quitIntro)
            {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                        quitIntro = true;
                    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                        phase = "Entry";
                        quitIntro = true;
                    }
                }
                gIntroscreen.render(0,0,NULL);

                SDL_Rect* currentPressFrame = &gPressClip[pressFrame/16];
                gPressEnter2start.render(100,SCREEN_HEIGHT *3 /4,currentPressFrame);

                pressFrame++;
                if(pressFrame/16 >= 2)
                {
                    pressFrame = 0;
                }

                SDL_Color color = {255, 255, 255};
                std::string scoreText = "Highscore: " + std::to_string(highscore);
                SDL_Surface* scoresurface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
                SDL_Texture* scoretexture = SDL_CreateTextureFromSurface(gRenderer, scoresurface);
                SDL_FreeSurface(scoresurface);

                // render
                SDL_Rect dest = {SCREEN_WIDTH - 230 , SCREEN_HEIGHT /2 + 50 , 200, 50};
                SDL_RenderCopy(gRenderer, scoretexture, NULL, &dest);
                SDL_DestroyTexture (scoretexture);

                SDL_RenderPresent (gRenderer);
                SDL_Delay (16);


            }
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);
        }

    //entrance animation
    if (phase == "Entry")
    {
        while (!quitentrance)
        {
            while (SDL_PollEvent (&e))
            {
                if (e.type == SDL_QUIT)
                {
                    quitentrance = true;
                    quit = true;
                }
                else
                {
                    phase = "Entry";
                }
            }
                Scrollingoffset +=2;
                if (Scrollingoffset > SCREEN_HEIGHT)
                {
                    Scrollingoffset = 0;
                }
                gBackGround.render (0,Scrollingoffset,NULL,1);
                gBackGround.render (0,Scrollingoffset - SCREEN_HEIGHT, NULL, 1);

                Start_posY -= 5;
                gPlane.render (posX, Start_posY, &gPlaneClips[2], 0);

                SDL_RenderPresent (gRenderer);

                if (Start_posY <= posY)
                {
                    quitentrance = true;
                    phase = "Start";
                }
            SDL_Delay(16);
        }
    }
    if (phase == "Start")
    {

        while (SDL_PollEvent(&e) != 0) {
        SDL_RenderClear(gRenderer);

            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else
            {
                if ( e.type == SDL_KEYDOWN && !e.key.repeat/**Tránh nhận lại phím nhiều lần khi bị giữ **/ )
                {
                    keyHeld [e.key.keysym.scancode] = true;
                }
                else if (e.type == SDL_KEYUP)
                {
                    keyHeld [e.key.keysym.scancode] = false;
                }
            }
        }

        Scrollingoffset +=2;
        if ( Scrollingoffset > 2*SCREEN_HEIGHT)
        {
            Scrollingoffset = 0;
        }
        gBackGround.render(0,Scrollingoffset,NULL,1);
        gBackGround2.render(0,Scrollingoffset - SCREEN_HEIGHT,NULL,1);
        gBackGround.render(0,Scrollingoffset - 2*SCREEN_HEIGHT,NULL,1);

        SDL_Color color = {0, 0, 0};
        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Surface* scoresurface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
        SDL_Texture* scoretexture = SDL_CreateTextureFromSurface(gRenderer, scoresurface);
        SDL_FreeSurface(scoresurface);

        // render
        SDL_Rect dest = {10, 10, 200, 50};
        SDL_RenderCopy(gRenderer, scoretexture, NULL, &dest);
        SDL_DestroyTexture (scoretexture);

        //Enemy
        if (counting % 3 == 0)
        {
            currentType = 4;
        }
        switch (currentType){
            case 1:
                enemyspawntimer++;
                if (enemyspawntimer >= 60 && !enemy1max && Enemyadded <9)
            {
                int spawnX = rand() % (SCREEN_WIDTH - ENEMY_WIDTH);
                int spawnY = rand() % (SCREEN_HEIGHT / 4);
                Enemy1 newEnemy (spawnX, spawnY);
                if ( Enemyadded >= 8)
                {
                    enemy1max = true;
                }

                if (newEnemy.loadfromfile("ABC/Enemy1.png"))
                {
                    enemies.push_back(newEnemy);
                    Enemyadded ++;

                }
                enemyspawntimer = 0;
            }

            for (Enemy1&q : enemies)
            {
                q.movement();
                q.render();
            }

            break;

            case 2:
                if (!enemy2max)
                {
                    int spawnX = 70;
                    int spawnY = -60;
                    for (int i=0; i<4; i++)
                    {
                        Enemy2 newEnemy (spawnX,spawnY);
                        spawnX += 100;
                        if (newEnemy.loadfromfile("ABC/Enemy2.png"))    enemies2.push_back(newEnemy);
                    }
                    spawnX = 70;
                    spawnY = -120;
                    for (int i=4; i<8; i++)
                    {
                        Enemy2 newEnemy (spawnX,spawnY);
                        spawnX += 100;
                        if (newEnemy.loadfromfile("ABC/Enemy2.png"))    enemies2.push_back(newEnemy);
                    }
                    enemy2max = true;
                }
                if (!entrancePlayed)
                {
                    bool allEntranceDone = true;
                    for (int i = 0; i < 4; i++) {
                        if (!enemies2[i].entranceDone) {
                            enemies2[i].render();
                            enemies2[i].entrance(1,7); //(type,speed)
                            allEntranceDone = false;
                        }
                    }
                    for (int i = 4; i < 8; i++) {
                        if (!enemies2[i].entranceDone) {
                            enemies2[i].render();
                            enemies2[i].entrance(2,7);
                            allEntranceDone = false;
                        }
                    }

                    if (allEntranceDone) {
                        entrancePlayed = true;
                    }
                }
                else
                {
                    for (Enemy2& q: enemies2)
                    {
                        q.render();
                        q.movement();
                    }
                }

                break;
            case 3:
                if (!enemy3max)
                {
                    int spawnX = -60;
                    int spawnY = SCREEN_HEIGHT/4 - 30;
                    for (int i=0; i<4; i++)
                    {
                        Enemy3 newEnemy (spawnX, spawnY);
                        spawnX -= 100;
                        spawnY += 23;
                        if (newEnemy.loadFromfile("ABC/Enemy3.png"))
                        {
                            enemies3Left.push_back(newEnemy);
                        }
                    }
                    spawnX = SCREEN_WIDTH + 60;
                    spawnY = SCREEN_HEIGHT/4 - 30;
                    for (int i=0;i<4;i++)
                    {
                        Enemy3 newEnemy (spawnX,spawnY);
                        spawnX += 100;
                        spawnY += 23;
                        if (newEnemy.loadFromfile("ABC/Enemy3.png"))
                        {
                            enemies3Right.push_back(newEnemy);
                        }
                    }
                    timer = SDL_GetTicks();
                    enemy3max = true;
                }
                else
                {
                    if (enemy3phase == 1)
                    {
                        if (SDL_GetTicks() - timer <= 17000)
                        {
                            for (int i =0 ;i<enemies3Left.size(); i++)
                            {
                                enemies3Left[i].render();
                                enemies3Left[i].movementType1("Left");
                            }
                            for (int i =0 ;i<enemies3Right.size(); i++)
                            {
                                enemies3Right[i].render();
                                enemies3Right[i].movementType1("Right");
                            }
                        }
                        else
                        {
                            enemy3phase = 2;
                            timer = SDL_GetTicks();
                        }
                    }
                    else if (enemy3phase == 2)
                    {
                        if (SDL_GetTicks() - timer <= 2100)
                        {
                            for (int i =0 ;i<enemies3Left.size(); i++)
                            {
                                enemies3Left[i].render();
                                enemies3Left[i].movementType2("Left");
                            }
                            for (int i =0 ;i<enemies3Right.size(); i++)
                            {
                                enemies3Right[i].render();
                                enemies3Right[i].movementType2("Right");
                            }
                        }
                        else
                        {
                            enemy3phase = 1;
                            timer = SDL_GetTicks();
                            for (int i=0; i<enemies3Left.size(); i++)
                            {
                                enemies3Left[i].resetposY(i);
                            }
                            for (int i=0; i<enemies3Right.size(); i++)
                            {
                                enemies3Right[i].resetposY(i);
                            }
                        }
                    }
                }
                break;

            case 4:
                if (!bossmax)
                {
                    int PosX = (SCREEN_WIDTH-BOSS_WIDTH)/2;
                    int PosY = SCREEN_HEIGHT/10;
                    Boss newBoss (PosX, PosY);
                    if (newBoss.loadfromfile("ABC/Boss.png"))
                    {
                        boss.push_back(newBoss);
                    }
                    bossmax = true;
                }
                else
                {
                    if (boss[0].HP > 1000)
                    {
                    if (boss[0].HP == 3500)
                    {
                        isspecial = SDL_GetTicks();
                        specialshot = SDL_GetTicks();
                    }
                    if (SDL_GetTicks() - isspecial <= 3000)
                    {
                        special = true;
                    }
                    else
                    {
                        special = false;
                    }

                    if (!special)
                    {
                        firetimer++;
                        if ( (firetimer)/50 % 2 == 0)
                        {
                            currentBossFrame = 0;
                        }
                        else
                        {
                            currentBossFrame = 1;
                        }
                        for (Boss& b: boss)
                        {
                            b.movement(special);
                            b.render(b.getX(), b.getY(), &BossClip[currentBossFrame]);
                        }
                    }
                    else
                    {
                        currentBossFrame = 1;
                        if (SDL_GetTicks() - specialshot >= 900 && SDL_GetTicks() - specialshot <= 2400)
                        {
                            currentBossFrame = 0;
                        }
                        for (Boss& b: boss)
                        {
                            b.movement(special);
                            b.render(b.getX(), b.getY(), &BossClip[currentBossFrame]);
                        }
                    }
                    }
                    else
                    {
                        deadframe++;
                        currentBossFrame = 2;
                        if (deadframe%50 >= 0 && deadframe%50 <= 3)
                        {
                            currentBossFrame = 3;
                        }
                        for (Boss& b: boss)
                        {
                            b.movement(!special);
                            b.render(b.getX(), b.getY(), &BossClip[currentBossFrame]);
                        }
                    }
                }
        }

        //Enemy Bullet
        if (currentType != 4)
        {
            EB_cooldown++;
        }
        else
        {
            BB_cooldown++;
        }
        switch (currentType)
            {
                case 1:
                    //add
                    if (EB_cooldown >= EB_INTERVAL)
                    {
                        for (int i =0; i<enemies.size(); i++)
                        {
                            enemybullet.push_back(EnemyBullet (enemies[i].getBox().x, enemies[i].getBox().y, ENEMY_WIDTH));
                        }
                        EB_cooldown = 0;
                    }
                    //erase
                    for (int i=0; i<enemybullet.size(); i++)
                    {
                        enemybullet[i].movement();
                        if (enemybullet[i].isOffscreen())
                        {
                            enemybullet.erase(enemybullet.begin() + i);
                            --i;
                        }
                    }

                    //render
                    for (EnemyBullet& i : enemybullet)
                    {
                        i.render();
                    }
                break;

                case 2:
                    //add
                    if (EB_cooldown >= EB_INTERVAL)
                    {
                        for (int i =0; i<enemies2.size(); i++)
                        {
                            enemybullet.push_back(EnemyBullet (enemies2[i].getBox().x, enemies2[i].getBox().y, ENEMY2_WIDTH));
                        }
                        EB_cooldown = 0;
                    }
                    //erase
                    for (int i=0; i<enemybullet.size(); i++)
                    {
                        enemybullet[i].movement();
                        if (enemybullet[i].isOffscreen())
                        {
                            enemybullet.erase(enemybullet.begin() + i);
                            --i;
                        }
                    }

                    //render
                    for (EnemyBullet& i : enemybullet)
                    {
                        i.render();
                    }
                break;

                case 3:
                    if (enemy3phase == 1)
                    {
                        //add bullet
                        if (EB_cooldown >= EB_INTERVAL - 15)
                        {
                            for (int i =0; i<enemies3Left.size(); i++)
                            {
                                eb3Left.push_back(EnemyBullet (enemies3Left[i].getBox().x, enemies3Left[i].getBox().y, ENEMY_WIDTH));
                            }
                            for (int i=0; i<enemies3Right.size(); i++)
                            {
                                eb3Right.push_back(EnemyBullet (enemies3Right[i].getBox().x, enemies3Right[i].getBox().y, ENEMY_WIDTH));
                            }
                            EB_cooldown = 0;
                        }
                        //erase
                        for (int i=0; i<eb3Left.size(); i++)
                        {
                            eb3Left[i].movement();
                            if (eb3Left[i].isOffscreen())
                            {
                                eb3Left.erase(eb3Left.begin() + i);
                                --i;
                            }
                        }
                        for (int i=0; i<eb3Right.size(); i++)
                        {
                            eb3Right[i].movement();
                            if (eb3Right[i].isOffscreen())
                            {
                                eb3Right.erase(eb3Right.begin() + i);
                                --i;
                            }
                        }

                        //render
                        for (EnemyBullet& i : eb3Left)
                        {
                            i.render();
                        }
                        for (EnemyBullet& i : eb3Right)
                        {
                            i.render();
                        }

                    }
                    else
                    {
                        eb3Left.clear();
                        eb3Right.clear();
                        EB_cooldown = 0;
                    }
                break;

                case 4:
                    if (!special)
                    {

                        if (currentBossFrame == 0 && firetimer >= 100 && firetimer%100 == 5)
                        {
                            BossBullet newbullet (boss[0].getX() + boss[0].getBox().w /2 -20, boss[0].getY()+boss[0].getBox().h);
                            if (newbullet.loadfromfile("ABC/BossBullet.png"))
                            {
                                bossBullet.push_back(newbullet);
                            }
                        }

                        for (int i=0; i<bossBullet.size(); i++)
                        {
                            bossBullet[i].movement();
                            if (bossBullet[i].isOffscreen())
                            {
                                bossBullet.erase(bossBullet.begin()+i);
                                --i;
                            }
                        }

                        for (BossBullet& b:bossBullet)
                        {
                            b.render();
                        }
                    }
                    else
                    {
                        if (currentBossFrame == 0 && BB_cooldown%10 == 0)
                        {
                            BossBullet newbullet (boss[0].getX() + boss[0].getBox().w /2 - 20, boss[0].getY()+boss[0].getBox().h);
                            if (newbullet.loadfromfile("ABC/BossBullet.png"))
                            {
                                bossBullet.push_back(newbullet);
                            }
                        }

                        for (int i=0; i<bossBullet.size(); i++)
                        {
                            bossBullet[i].movement();
                            if (bossBullet[i].isOffscreen())
                            {
                                bossBullet.erase(bossBullet.begin()+i);
                                --i;
                            }
                        }

                        for (BossBullet& b:bossBullet)
                        {
                            b.render();
                        }
                    }
            }
        //level design

        if (!currentWaveCleared)
        {
            if (currentType == 1 && enemies.empty() && enemy1max && Enemyadded >=8)
            {
                currentWaveCleared = true;
                waveWaiting = true;
                waveClearTime = SDL_GetTicks();
            }
            else if (currentType == 2 && enemies2.empty() && enemy2max)
            {
                currentWaveCleared = true;
                waveWaiting = true;
                waveClearTime = SDL_GetTicks();
            }
            else if (currentType == 3 && enemies3Left.empty() && enemies3Right.empty() && enemy3max)
            {
                currentWaveCleared = true;
                waveWaiting = true;
                waveClearTime = SDL_GetTicks();
            }
            else if (currentType == 4 && boss.empty() && bossmax)
            {
                currentWaveCleared = true;
                waveWaiting = true;
                waveClearTime = SDL_GetTicks();
            }
        }
        else if (!readyToChangeType)
        {
            if (SDL_GetTicks() - waveClearTime >= 6000) // Delay 10s
            {
                readyToChangeType = true;
            }
        }

        if (readyToChangeType && SDL_GetTicks() - waveClearTime >= 6000)
        {
            readyToChangeType = false;

            int newType;
            do
            {
                newType = rand()%3 +1;
            }
            while (newType == currentType);

            currentType = newType;

            // reset
            currentWaveCleared = false;
            enemy1max = false;
            enemy2max = false;
            enemy3max = false;
            entrancePlayed = false;
            enemies.clear();
            enemies2.clear();
            enemies3Left.clear();
            enemies3Right.clear();
            timer = 0;
            Enemyadded = 0;
            counting ++;
            firetimer = 0;
            bossmax = false;
            boss.clear();
            special = false;
            isspecial = 0;
            specialshot = 0;
            deadframe = 0;

            //new wave
            waveClearTime = SDL_GetTicks();
            }

        //Plane Bullet
        Bulletscooldown++;
        if (Bulletscooldown >= BULLETS_INTERVAL)
        {
            bullets.push_back(Bullet(posX, posY));
            shoot = Mix_LoadWAV("ABC/shooting.ogg");
                if (!shoot) {
                    std::cout << "Failed to load sound effect: " << Mix_GetError() << std::endl;
                }

                // Khi bắn:
                Mix_VolumeChunk (shoot, MIX_MAX_VOLUME / 2);
                Mix_PlayChannel(-1, shoot, 0); //avoid loop
            Bulletscooldown = 0;
        }

        for (int i =0; i< bullets.size();i++)
        {
            bullets[i].movement();
            if (bullets[i].isOffscreen())
            {
                bullets.erase(bullets.begin() + i);
                --i;
            }
        }
        for (Bullet&b : bullets)
            {
                b.render();
            }
        }

        //Gameplay screen

        if (keyHeld [SDL_SCANCODE_DOWN] && posY <= (SCREEN_HEIGHT-64))
        {
            posY += 5;
        }
        if (keyHeld [SDL_SCANCODE_UP] && posY >= 0)
        {
            posY -= 5;
        }
        if (keyHeld [SDL_SCANCODE_RIGHT] && posX <= (SCREEN_WIDTH-32))
        {
            posX += 5;
        }
        if (keyHeld [SDL_SCANCODE_LEFT] && posX >= 0)
        {
            posX -= 5;
        }

        if (keyHeld [SDL_SCANCODE_LEFT] && !keyHeld[SDL_SCANCODE_RIGHT])
        {
            leftheldframe++;
            rightheldframe = 0;

            if (leftheldframe >= 15) currentplaneFrame =0;
            else if (leftheldframe >= 5) currentplaneFrame =1;
            else currentplaneFrame = 2;
        }

        else if (keyHeld [SDL_SCANCODE_RIGHT] && !keyHeld[SDL_SCANCODE_LEFT])
        {
            leftheldframe =0;
            rightheldframe++;

            if (rightheldframe >=15) currentplaneFrame =4;
            else if (rightheldframe >=5) currentplaneFrame =3;
            else currentplaneFrame = 2;
        }
        else
        {
            leftheldframe = 0;
            rightheldframe = 0;
            currentplaneFrame = 2;
        }

        SDL_Rect* currentplaneClip = &gPlaneClips[currentplaneFrame];
        gPlane.render(posX, posY, currentplaneClip, 0);

        /**check plane-enemies/bullets collision**/
        SDL_Rect PlaneBox = {posX, posY, PLANE_WIDTH, PLANE_HEIGHT};
        //Plane-bossbullet
        for (int i=0 ;i< bossBullet.size(); i++)
        {
            if (checkCollision (bossBullet[i].getBox(), PlaneBox))
            {
                gPlane.planeHP -= bossBullet[i].damage;
                bossBullet.erase(bossBullet.begin() + i);
            }
        }

        //Plane-enemies
        for (int i=0; i<enemies.size(); i++)
        {
            if (checkCollision (enemies[i].getBox(), PlaneBox))
            {
                //striked
                enemies[i].HP -= 100; // 1 bullets' damage
                gPlane.planeHP -= 100;
                if (enemies[i].HP <= 0)
                {
                    score += 100;
                    enemies[i].free();
                    enemies.erase(enemies.begin() + i);
                }
            }
        }
        for (int i=0; i<enemies2.size();i++)
        {
            if (checkCollision (enemies2[i].getBox(), PlaneBox))
            {
                //striked
                enemies2[i].HP -= 100; // 1 bullets' damage
                gPlane.planeHP -= 100;
                if (enemies2[i].HP <= 0)
                {
                    score += 100;
                    enemies2[i].free();
                    enemies2.erase(enemies2.begin() + i);
                }
            }
        }
        for (int i=0; i<enemies3Left.size();i++)
        {
            if (checkCollision (enemies3Left[i].getBox(), PlaneBox))
            {
                //striked
                enemies3Left[i].HP -= 100; // 1 bullets' damage
                gPlane.planeHP -= 100;
                if (enemies3Left[i].HP <= 0)
                {
                    score += 200;
                    enemies3Left[i].free();
                    enemies3Left.erase(enemies3Left.begin() + i);
                }
            }
        }
        for (int i=0; i<enemies3Right.size();i++)
        {
            if (checkCollision (enemies3Right[i].getBox(), PlaneBox))
            {
                //striked
                enemies3Right[i].HP -= 100; // 1 bullets' damage
                gPlane.planeHP -= 100;
                if (enemies3Right[i].HP <= 0)
                {
                    score += 200;
                    enemies3Right[i].free();
                    enemies3Right.erase(enemies3Right.begin() + i);
                }
            }
        }

        //Bullet-plane
        for (int i=0; i<enemybullet.size(); i++)
        {
            if (checkCollision (PlaneBox, enemybullet[i].getBox()))
            {
                gPlane.planeHP -= enemybullet[i].damage;
                enemybullet.erase(enemybullet.begin() + i);
                --i;
            }
        }
        for (int i=0; i<eb3Left.size(); i++)
        {
            if (checkCollision (PlaneBox, eb3Left[i].getBox()))
            {
                gPlane.planeHP -= eb3Left[i].damage;
                eb3Left.erase(eb3Left.begin() + i);
                --i;
            }
        }
        for (int i=0; i<eb3Right.size(); i++)
        {
            if (checkCollision (PlaneBox, eb3Right[i].getBox()))
            {
                gPlane.planeHP -= eb3Right[i].damage;
                eb3Right.erase(eb3Right.begin() + i);
                --i;
            }
        }

        /**Check enemies-bullets' collision**/
        for (int i = 0; i < bullets.size(); ++i)
        {
            bool bulletErased = false;

            // enemies
            for (int j = 0; j < enemies.size(); ++j)
            {
                if (checkCollision(bullets[i].getBox(), enemies[j].getBox()))
                {
                    //if striked
                    enemies[j].HP -= bullets[i].damage;
                    bullets.erase(bullets.begin() + i);
                    --i;
                    bulletErased = true;

                    //check enemy's HP
                    if (enemies[j].HP <= 0)
                    {
                        score += 100;
                        enemies[j].free();
                        enemies.erase(enemies.begin() + j);
                    }
                    break;
                }
            }
            if (bulletErased) continue;

            // enemies2
            for (int j = 0; j < enemies2.size(); ++j)
            {
                if (checkCollision(bullets[i].getBox(), enemies2[j].getBox()) && entrancePlayed)
                {
                    enemies2[j].HP -= bullets[i].damage;
                    bullets.erase(bullets.begin() + i);
                    --i;
                    bulletErased = true;

                    if (enemies2[j].HP <= 0)
                    {
                        score += 100;
                        enemies2[j].free();
                        enemies2.erase(enemies2.begin() + j);
                    }
                    break;
                }
            }
            if (bulletErased) continue;

            // enemies3Left
            for (int j = 0; j < enemies3Left.size(); ++j)
            {
                if (checkCollision(bullets[i].getBox(), enemies3Left[j].getBox()))
                {
                    enemies3Left[j].HP -= bullets[i].damage;
                    bullets.erase(bullets.begin() + i);
                    --i;
                    bulletErased = true;

                    if (enemies3Left[j].HP <= 0)
                    {
                        score += 200;
                        enemies3Left[j].free();
                        enemies3Left.erase(enemies3Left.begin() + j);
                    }
                    break;
                }
            }
            if (bulletErased) continue;

            // enemies3right
            for (int j=0; j < enemies3Right.size(); j++)
            {
                if (checkCollision(bullets[i].getBox(), enemies3Right[j].getBox()))
                {
                    enemies3Right[j].HP -=  bullets[i].damage;
                    bullets.erase(bullets.begin() + i);
                    --i;
                    bulletErased = true;
                    if (enemies3Right[j].HP <= 0)
                    {
                        score += 200;
                        enemies3Right[j].free();
                        enemies3Right.erase(enemies3Right.begin() +j);
                    }
                    break;
                }
            }

            if (bulletErased) continue;

            //boss
            for (int j=0; j<boss.size(); j++)
            {
                if (checkCollision(bullets[i].getBox(), boss[j].getBox()))
                {
                    boss[j].HP -=  bullets[i].damage;
                    bullets.erase(bullets.begin() + i);
                    --i;
                    bulletErased = true;
                    if (boss[j].HP <= 0)
                    {
                        score += 1000;
                        boss[j].free();
                        boss.erase(boss.begin() +j);
                    }
                    break;
                }
            }
        }
        SDL_RenderPresent(gRenderer);

       if (gPlane.planeHP <= 0)
        {
            if (score > highscore)
            {
            SaveHighScore(score);
            highscore = score;
            }

            phase = "Game_over";
            SDL_RenderClear (gRenderer);
        }

        if (phase == "Game_over")
        {
                gameoversound = Mix_LoadWAV("ABC/Gameover.wav");
                if (!gameoversound) {
                    std::cout << "Failed to load sound effect: " << Mix_GetError() << std::endl;
                }

                // Khi bắn:
                Mix_PlayChannel(-1, gameoversound, 0);

            while ( !quitRestart)
            {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                        quitRestart = true;
                    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                        phase = "Intro";
                        quitRestart = true;
                        quit = true;
                    }
                }
                Gameoverscreen.render(0,0,NULL);

                SDL_Rect* currentPressFrame = &gRestartClip[GameoverFrame/16];
                Press2restart.render(73 ,SCREEN_HEIGHT *3 /4,currentPressFrame);

                GameoverFrame++;
                if(GameoverFrame/16 >= 2)
                {
                    GameoverFrame = 0;
                }

                if (score >= highscore)
                {
                    SDL_Color color = {0, 0, 0};
                    std::string scoreText = "New record: " + std::to_string(score);
                    SDL_Surface* scoresurface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
                    SDL_Texture* scoretexture = SDL_CreateTextureFromSurface(gRenderer, scoresurface);
                    SDL_FreeSurface(scoresurface);

                    // render
                    SDL_Rect dest = {100, SCREEN_HEIGHT /2 + 70, 300, 50};
                    SDL_RenderCopy(gRenderer, scoretexture, NULL, &dest);
                    SDL_DestroyTexture (scoretexture);
                }

                SDL_RenderPresent (gRenderer);
                SDL_Delay (16);
            }
        }
    }
    Mix_FreeMusic(bgm);
    Mix_FreeChunk(shoot);
    Mix_FreeChunk(gameoversound);

    close();
    return 0;
}
