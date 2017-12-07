#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/time.h>

#include <GL/glut.h>

#define GRAVITY -0.001f
#define frand() ((float) rand() / (RAND_MAX+1.0))

char title[] = "Wall-e: Adventure";

int winWidth = 800, winHeight = 600, winPosX = 200, winPosY = 50;

struct timespec start, stop;

struct obstacle{
    float x;
    float y;
    float width;
    float height;
    int type;
};

struct player{
    float x;
    float y;
    float score;
    float velocityY;
    double velocityX;
    float acelerationY;
    float acelerationX;
    float height;
    float width;
    float headY;
    float headDown;
};

struct world{
    float gravity;
    float bestScore;
    int bestScorePrint;
    float groundX;
    float groundY;
    int gameState;
    int obs;
    int frame;
    float sky[3];
    FILE *file;
    float dy;
    float dx;
    float dy_min;
    float dx_min;
};

struct player p;
struct obstacle obstacles[3];
struct world game;

void drawGrid(void){
    // grade para facilitar desenvolvimento
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
      int i;
      for (i = -6; i<7; i++){
          glVertex3f(1.0f * i, -7.0f, 0.0f);
          glVertex3f(1.0f * i,  7.0f, 0.0f);
          
          glVertex3f(-7.0f, 1.0f * i, 0.0f);
          glVertex3f( 7.0f, 1.0f * i, 0.0f);
      }
      
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(-1.5f, 0.0f, 0.0f);
      glVertex3f( 1.5f, 0.0f, 0.0f);
      
      glVertex3f(0.0f,  1.5f, 0.0f);
      glVertex3f(0.0f, -1.5f, 0.0f);
    glEnd();
}

void drawSky(void){
    
    glBegin(GL_QUADS);
      glColor3f(game.sky[0], game.sky[1], game.sky[2]);
      glVertex3f(-7.0f,  7.0f, 0.0f);
      glVertex3f(-7.0f, -7.0f, 0.0f);
      glVertex3f( 7.0f, -7.0f, 0.0f);
      glVertex3f( 7.0f,  7.0f, 0.0f);
    glEnd();
}
void drawGround(void){
    
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
      glVertex3f(-7.0f, game.groundY       , 0.0f);
      glVertex3f(-7.0f, game.groundY - 1.0f, 0.0f);
      glVertex3f( 7.0f, game.groundY - 1.0f, 0.0f);
      glVertex3f( 7.0f, game.groundY       , 0.0f);
    glEnd();
    
    if(game.groundX <= -2.0f)game.groundX = 0.0f;
    
    int i;
    for (i = 0; i<9; i++){
        glColor3f(0.0f, 0.8f, 0.0f);
        glBegin(GL_QUADS);
          glVertex3f(-7.0f + (i * 2) + game.groundX, -4.00f, 0.0f);
          glVertex3f(-7.0f + (i * 2) + game.groundX, -4.25f, 0.0f);
          glVertex3f(-7.5f + (i * 2) + game.groundX, -4.25f, 0.0f);
          glVertex3f(-7.5f + (i * 2) + game.groundX, -4.00f, 0.0f);
        glEnd();
        
        glColor3f(0.3f, 0.6f, 0.0f);
        glBegin(GL_QUADS);
          glVertex3f(-8.5f + (i * 2) + game.groundX, -4.25f, 0.0f);
          glVertex3f(-8.5f + (i * 2) + game.groundX, -4.50f, 0.0f);
          glVertex3f(-8.0f + (i * 2) + game.groundX, -4.50f, 0.0f);
          glVertex3f(-8.0f + (i * 2) + game.groundX, -4.25f, 0.0f);
        glEnd();
        
        glColor3f(0.0f, 0.6f, 0.0f);
        glBegin(GL_QUADS);
          glVertex3f(-8.25f + (i * 2) + game.groundX, -4.75f, 0.0f);
          glVertex3f(-8.25f + (i * 2) + game.groundX, -5.00f, 0.0f);
          glVertex3f(-7.50f + (i * 2) + game.groundX, -5.00f, 0.0f);
          glVertex3f(-7.50f + (i * 2) + game.groundX, -4.75f, 0.0f);
        glEnd();
    }
}
void drawPlayer(player){
    
    glColor3f(0.6f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
      glVertex3f(p.x - 0.125f, p.y + p.headY + 0.0f, 0.0f);
      glVertex3f(p.x - 0.125f, p.y + p.headY - 0.5f, 0.0f);
      glVertex3f(p.x + 0.125f, p.y + p.headY - 0.5f, 0.0f);
      glVertex3f(p.x + 0.125f, p.y + p.headY + 0.0f, 0.0f);
    glEnd();
    
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
      glVertex3f(p.x - 0.5f, p.y + p.headY + 0.5f, 0.0f);
      glVertex3f(p.x - 0.5f, p.y + p.headY + 0.0f, 0.0f);
      glVertex3f(p.x + 0.5f, p.y + p.headY + 0.0f, 0.0f);
      glVertex3f(p.x + 0.5f, p.y + p.headY + 0.5f, 0.0f);
    glEnd();
    
    glColor3f(0.4f, 0.4f, 1.0f);
    glBegin(GL_QUADS);
      glVertex3f(p.x + 0.125f, p.y + p.headY + 0.375f, 0.0f);
      glVertex3f(p.x + 0.125f, p.y + p.headY + 0.125f, 0.0f);
      glVertex3f(p.x + 0.500f, p.y + p.headY + 0.125f, 0.0f);
      glVertex3f(p.x + 0.500f, p.y + p.headY + 0.375f, 0.0f);
    glEnd();
    
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
      glVertex3f(p.x - (p.width / 2.0f), p.y + (p.height / 2.0f)         , 0.0f);
      glVertex3f(p.x - (p.width / 2.0f), p.y - (p.height / 2.0f) + 0.125f, 0.0f);
      glVertex3f(p.x + (p.width / 2.0f), p.y - (p.height / 2.0f) + 0.125f, 0.0f);
      glVertex3f(p.x + (p.width / 2.0f), p.y + (p.height / 2.0f)         , 0.0f);
    glEnd();
    
    glColor3f(0.5f, 0.25f, 0.0f);
    glBegin(GL_POLYGON);
      glVertex3f(p.x - 0.500f, p.y         , 0.0f);
      glVertex3f(p.x - 0.625f, p.y - 0.125f, 0.0f);
      glVertex3f(p.x - 0.625f, p.y - 0.375f, 0.0f);
      glVertex3f(p.x - 0.375f, p.y - 0.500f, 0.0f);
      glVertex3f(p.x + 0.375f, p.y - 0.500f, 0.0f);
      glVertex3f(p.x + 0.625f, p.y - 0.375f, 0.0f);
      glVertex3f(p.x + 0.625f, p.y - 0.125f, 0.0f);
      glVertex3f(p.x + 0.250f, p.y         , 0.0f);
    glEnd();
    
    glColor3f(0.62f, 0.37f, 0.0f);
    glBegin(GL_POLYGON);
      glVertex3f(p.x - 0.500f * 0.8, p.y - 0.125f * 0.8, 0.0f);
      glVertex3f(p.x - 0.500f * 0.8, p.y - 0.375f * 0.8, 0.0f);
      glVertex3f(p.x - 0.375f * 0.8, p.y - 0.500f * 0.8, 0.0f);
      glVertex3f(p.x + 0.375f * 0.8, p.y - 0.500f * 0.8, 0.0f);
      glVertex3f(p.x + 0.500f * 0.8, p.y - 0.375f * 0.8, 0.0f);
      glVertex3f(p.x + 0.500f * 0.8, p.y - 0.125f * 0.8, 0.0f);
    glEnd();
}

void drawObstacles(struct obstacle o){
    
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
      glVertex3f(o.x - (o.width / 2), o.y - (o.height / 2), 0.0f);
      glVertex3f(o.x - (o.width / 2), o.y + (o.height / 2), 0.0f);
      glVertex3f(o.x + (o.width / 2), o.y + (o.height / 2), 0.0f);
      glVertex3f(o.x + (o.width / 2), o.y - (o.height / 2), 0.0f);
    glEnd();
}

void drawScore(void){
    // desenha a pontuacao no topo da tela
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
      glVertex3f(1.0f, 4.8f, 0.0f);
      glVertex3f(1.0f, 4.0f, 0.0f);
      glVertex3f(4.8f, 4.0f, 0.0f);
      glVertex3f(4.8f, 4.8f, 0.0f);
    glEnd();
    
    char str[5] = "";
    char str2[5] = "";
    
    int s = p.score;
    int bs = game.bestScore;
    
    sprintf(str, "%d", s);
    sprintf(str2, "%d", bs);
    
    glColor3f(0.9f, 0.5f, 0.0f);
    glRasterPos2f(3.2f, 4.2f);
    
    unsigned int length = sizeof(str);
    unsigned int i;
    for (i = 0; i < length; i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
    
    glRasterPos2f(1.2f, 4.2f);
    for (i = 0; i < length; i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str2[i]);
    }
    
    //Quando houver um novo record, 
    //serah impresso na tela 'Best Score' em baixo do menu de gameover
    if (game.bestScorePrint == 1) {
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
          glVertex3f(-1.0f, -1.5f, 0.0f);
          glVertex3f(-1.0f, -2.1f, 0.0f);
          glVertex3f( 1.0f, -2.1f, 0.0f);
          glVertex3f( 1.0f, -1.5f, 0.0f);
        glEnd();
        
        char str[] = "Best Score";
        
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(-0.9f, -2.0f);
        length = sizeof(str);
        for (i = 0; i < length; i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
        }
    }
    
}

void drawMenu(void){
    // menu inicial
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
      glVertex3f(-7.0f,  7.0f, 0.0f);
      glVertex3f(-7.0f, -7.0f, 0.0f);
      glVertex3f( 7.0f, -7.0f, 0.0f);
      glVertex3f( 7.0f,  7.0f, 0.0f);
    glEnd();
    
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_POLYGON);
      glVertex3f(-1.0f,  1.5f, 0.0f);
      glVertex3f(-1.5f,  1.0f, 0.0f);
      glVertex3f(-1.5f, -1.0f, 0.0f);
      glVertex3f(-1.0f, -1.5f, 0.0f);
      glVertex3f( 1.0f, -1.5f, 0.0f);
      glVertex3f( 1.5f, -1.0f, 0.0f);
      glVertex3f( 1.5f,  1.0f, 0.0f);
      glVertex3f( 1.0f,  1.5f, 0.0f);
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
      glVertex3f(-0.75f,  1.0f, 0.0f);
      glVertex3f(-0.75f, -1.0f, 0.0f);
      glVertex3f( 1.0f, -0.0f, 0.0f);
    glEnd();
}

void drawGameOver(void){
    // tela de gameover (menu flutante de play)
    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_POLYGON);
      glVertex3f(-1.0f,  1.5f, 0.0f);
      glVertex3f(-1.5f,  1.0f, 0.0f);
      glVertex3f(-1.5f, -1.0f, 0.0f);
      glVertex3f(-1.0f, -1.5f, 0.0f);
      glVertex3f( 1.0f, -1.5f, 0.0f);
      glVertex3f( 1.5f, -1.0f, 0.0f);
      glVertex3f( 1.5f,  1.0f, 0.0f);
      glVertex3f( 1.0f,  1.5f, 0.0f);
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.7f);
    glBegin(GL_TRIANGLES);
      glVertex3f(-0.75f,  1.0f, 0.0f);
      glVertex3f(-0.75f, -1.0f, 0.0f);
      glVertex3f( 1.0f, -0.0f, 0.0f);
    glEnd();
}

struct obstacle generateObstacle(float x) {
    //Gera e retorna um obstaculo
    struct obstacle obs;
    
    if (p.score >= 500 && frand() < 0.1){
        //Obstaculo superior (flutuando no ar)
        obs.type = 1;
        obs.x = x;
        obs.width = 1.0f;
        obs.height = 8.0f;
        obs.y = game.groundY + 1.75f + (obs.height / 2.0f);
    } else {
        //Obstaculo inferor (no chao)
        obs.type = 0;
        obs.x = x;
        obs.width = 0.5f + (frand() / 1.5f);// altura aleatoria
        obs.height = 1.0f + (frand() * 2.0f);// largura aleatoria
        obs.y = game.groundY + (obs.height / 2.0f);
    }
    return obs;
}

void display(void) {
    //desenha a composicao da tela
    
    // clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // reset transformations
    glLoadIdentity();
    
    // setting up the camera
    gluLookAt(0.0f, 0.0f, 10.0f,
              0.0f, 0.0f,  0.0f,
              0.0f, 1.0f,  0.0f);
    
    if (game.gameState == 0) {
        drawMenu();
    } else {
        drawSky();
        
        drawGround();
        
        drawObstacles(obstacles[0]);
        drawObstacles(obstacles[1]);
        drawObstacles(obstacles[2]);
        
        drawPlayer();
        
        if(game.gameState == 2){
            drawGameOver();
        }
    }
    
    drawScore();
    //drawGrid();//Util para guiar desenho
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    //Faz o enquadramento do cenario quando a janela eh redimencionada
    if (h == 0){
        h = 1;
    }
    
    float aspect = (float)w / (float)h;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    
    // set up orthographic projection view
    if (w >= h){
        glOrtho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, 0.1f, 100.0f);
    } else {
        glOrtho(-5.0f, 5.0f, -5.0f / aspect, 5.0f / aspect, 0.1f, 100.0f);
    }
    
    glMatrixMode(GL_MODELVIEW);
}

void init(void){
    
    game.gameState = 1;//Estado de RUN (rodando o jogo)
    game.bestScorePrint = 0;
    game.obs = 0;//Obstaculo q vai ser avaliado (colisao)
    
    //Estabelece os dados inicais do player ------------------------------------
    p.height = 1.0f;
    p.width = 1.0f;
    p.x = -4.0f;
    p.y = game.groundY - (p.height / 2.0f);
    p.acelerationX = -0.0000005;
    p.acelerationY = 0.0f;
    p.score = 0.0f;
    p.velocityX = -0.02;
    p.velocityY = 0.0f;
    p.headY = 1.0f;
    p.headDown = 0;
    // -------------------------------------------------------------------------
    
    // Cria os obstaculos ------------------------------------------------------
    obstacles[0] = generateObstacle(10.0f);
    obstacles[1] = generateObstacle(25.0f);
    obstacles[2] = generateObstacle(40.0f);
    // -------------------------------------------------------------------------
}

void saveBestScore(float score){
    
    int s = score;
    
    game.file = fopen("score.txt", "w");
    fprintf(game.file, "%d", s);
    fclose(game.file);
}

void gameover(void){
    
    game.gameState = 2;//Estado de Game Over
    
    //Verifica se ha um novo record --------------------------------------------
    if (p.score > game.bestScore) {
        game.bestScore = p.score;
        game.bestScorePrint = 1;
        saveBestScore(p.score);//salva em arquivo o novo record
    }
}

void run(void) {
    //variaveis para calculo da colisao ----------------------------------------
    game.dy = fabs(p.y - obstacles[game.obs].y);
    game.dx = fabs(p.x - obstacles[game.obs].x);
    game.dy_min = fabs((p.height / 2.0f) + (obstacles[game.obs].height / 2.0f));
    game.dx_min = fabs((p.width / 2.0f) + (obstacles[game.obs].width / 2.0f));
    // -------------------------------------------------------------------------
    
    if(game.gameState == 1){
        //chama gameover se houver colisao com um obstaculo
        if(
        
        ((obstacles[game.obs].type == 0)&&
            (game.dy <= game.dy_min)&&(game.dx <= game.dx_min))||
        ((obstacles[game.obs].type == 1)&&
            (1.75f <= (p.y-game.groundY+0.5f + p.headY))&&(game.dx <= game.dx_min))
        
        ){
            gameover();
        } else {
            //Movimento do cenario (obstaculos) --------------------------------
            p.velocityX += p.acelerationX;
            obstacles[0].x += p.velocityX;
            obstacles[1].x += p.velocityX;
            obstacles[2].x += p.velocityX;
            // -----------------------------------------------------------------
            
            //Animacao da cabeca do player -------------------------------------
            if(p.headDown == 0){
                if(p.headY < 1.0f){
                    p.headY += 0.005;
                    p.velocityX += 0.0001;
                }
            } else if(p.headY > 0.375f) {
                    p.headY -= 0.005;
                    p.velocityX -= 0.0001;
            }
            // -----------------------------------------------------------------
            
            //Fisica da gravidade ----------------------------------------------
            p.y += p.velocityY;
            p.velocityY += p.acelerationY;
            // -----------------------------------------------------------------
            
            p.score -= p.velocityX;//atualiza pontuacao
            
            //Muda a cor do ceu ------------------------------------------------
            int temp = (int)p.score % 2500;
            
            game.sky[0] = 0.0f;
            game.sky[1] = 0.0f;
            game.sky[2] = 1.0f;
            if(temp >= 500){
                game.sky[2] = 0.5f;
                if(temp >= 1000){
                    game.sky[2] = 0.0f;
                    if(temp >= 1500){
                        game.sky[0] = 1.0f;
                        game.sky[1] = 1.0f;
                        game.sky[2] = 1.0f;
                        if(temp >= 2000){
                            game.sky[0] = 0.8f;
                            game.sky[1] = 0.8f;
                            game.sky[2] = 0.0f;
                        }
                    }
                }
            }
            // -----------------------------------------------------------------
            
            //animacao do chao
            game.groundX += p.velocityX;
            
            //Controle da queda (colisao com o chao)
            if((p.y - (p.height / 2.0f))<=game.groundY){
                p.velocityY = 0.0f;
                p.acelerationY = 0.0f;
                p.y = game.groundY + (p.height/2.0f);
            }
            
            //Realocado os obstaculos a medida que passam pela tela ------------
            if(obstacles[0].x<-10.0f){
                obstacles[0] = generateObstacle(35.0f);
                game.obs = 1;
            }
            if(obstacles[1].x<-10.0f){
                obstacles[1] = generateObstacle(35.0f);
                game.obs = 2;
            }
            if(obstacles[2].x<-10.0f){
                obstacles[2] = generateObstacle(35.0f);
                game.obs = 0;
            }
            // -----------------------------------------------------------------
        }
    }
    
    display();
    game.frame++;
}

void pressKey(unsigned char key, int x, int y) {
    
    if (game.gameState == 0 || game.gameState == 2) {
        //iniciar ou reiniciar o jogo
        if(key == 32){
           init();
        }
    }else if (game.gameState == 1){
        //fazer o player pular
        if (key == 32){
            if (p.y <= game.groundY + (p.height/2)+0.01){
                p.acelerationY = game.gravity;
                p.velocityY = 0.110f;
            }
        }
    }
}

void pressSpecialKey(int key, int x, int y) {
    
    if (game.gameState == 1){
        if (key == GLUT_KEY_UP){
            //fazer o player pular
            if (p.y <= -3.0f){
                p.acelerationY = game.gravity;
                p.velocityY = 0.110f;
            }
        } else if (key == GLUT_KEY_DOWN){
            p.headDown = 1;//baixar a cabeca do player
        }
    }
}

void releaseSpecialKey(int key, int x, int y) {
    
    if (key == GLUT_KEY_DOWN){
        p.headDown = 0;//levantar a cabeca do player
    }
}

void showFrame(int x) {
    //Apenas para testes
    clock_gettime(CLOCK_MONOTONIC, &stop);
    printf("%lu :: %f - %f FPS\n", stop.tv_nsec - start.tv_nsec, game.frame*0.8f, game.frame*1.0f);
    game.frame = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    glutTimerFunc(1000, showFrame, 0);
}

int main(int argc, char *argv[]) {
    
    srand(time(NULL));
    
    //Inicializar janela -------------------------------------------------------
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(winPosX, winPosY);
    glutCreateWindow(title);
    // -------------------------------------------------------------------------
    
    // register callbacks ------------------------------------------------------
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(run);
    
    //timerStart = clock();
    //clock_gettime(CLOCK_MONOTONIC, &start);
    //glutTimerFunc(0, showFrame, 0);
    
    glutKeyboardFunc(pressKey);
    glutSpecialFunc(pressSpecialKey);
    glutSpecialUpFunc(releaseSpecialKey);
    glutIgnoreKeyRepeat(1);
    // -------------------------------------------------------------------------
    
    // Inicializando o mundo
    game.bestScore = 0.0f;
    game.frame = 0;
    //game.gameoverDelay = 0;
    game.gameState = 0;
    game.gravity = GRAVITY;
    game.groundX = 0.0f;
    game.groundY = -4.0f;
    game.obs = 0;
    game.sky[0] = 0.0f;//red
    game.sky[1] = 0.0f;//blue
    game.sky[2] = 1.0f;//green
    game.bestScorePrint = 0;
    
    //recupera a melhor pontuacao ----------------------------------------------
    game.file = fopen("score.txt", "r");
    if (game.file == NULL) {
       game.file = fopen("score.txt", "w");
       fprintf(game.file, "0");
       fclose(game.file);
    } else {
       fscanf(game.file, "%f", &game.bestScore);
       fclose(game.file);
    }
    // -------------------------------------------------------------------------
    
    // enter GLUT event processing cycle
    glutMainLoop();
    
	return 0;
}
