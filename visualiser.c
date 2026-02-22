#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include<time.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ARRAY_SIZE 100

void render_text(SDL_Renderer* renderer,TTF_Font* font,const char* text,int x,int y){
    SDL_Color color={255,255,255,255};
    SDL_Surface* surface=TTF_RenderText_Solid(font,text,color);
    SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,surface);

    SDL_Rect dest={x,y,surface->w,surface->h};
    SDL_RenderCopy(renderer,texture,NULL,&dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

}

void draw_array(SDL_Renderer* renderer,int arr[],int current_i,int current_j,int sweep)
{
    int bar_width=SCREEN_WIDTH/ARRAY_SIZE;

    for(int i=0;i<ARRAY_SIZE;i++){
        if(i<=sweep){
        SDL_SetRenderDrawColor(renderer,50,205,50,255);
        }
        else if(i==current_i){
            SDL_SetRenderDrawColor(renderer,0,255,255,255);
        }
        else if(i==current_j){
            SDL_SetRenderDrawColor(renderer,255,100,100,255);
        }
        else{
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
        }
        SDL_Rect rect={
            i*bar_width, //x position
            SCREEN_HEIGHT-arr[i], //y position
            bar_width-1, //width (with 1 px gap)
            arr[i] //height
        };
        SDL_RenderFillRect(renderer,&rect);
    }
}

int main(int argc,char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Sorting Visualiser",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    //Loading font
    TTF_Font* font=TTF_OpenFont("arial.ttf",24);
    if(!font){
        printf("Error loading font: %s\n",TTF_GetError());
    }
    
    int arr[ARRAY_SIZE];
    srand(time(NULL));
    for(int i=0;i<ARRAY_SIZE;i++)
    {
        arr[i] = rand()%(SCREEN_HEIGHT-100)+10;
    }

    int running=1;
    SDL_Event event;

    int i=1;
    int j=0;
    int key=arr[i];
    int is_sorting=0;
    int complete_sweep=-1;
    int comparisons = 0;

    //Timer variabke
    Uint32 start_time=0;
    Uint32 end_time=0;
    Uint32 last_time=0;
    float total_sorting_time=0.0f;
    bool timer_started=false;
    char time_text[32]="Time: 0.00s";

    while(running)
    {
        while (SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running=0;
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_SPACE){
                    is_sorting=!is_sorting;

                    last_time=SDL_GetTicks();
                    //Timer
                    if(is_sorting && !timer_started){
                        timer_started=true;
                    }
                    
                }
                else if(event.key.keysym.sym == SDLK_r){
                    for(int k=0;k<ARRAY_SIZE;k++){
                        arr[k]=rand()%(SCREEN_HEIGHT-100)+10;
                    }
                    i=1;
                    j=0;
                    key=arr[i];
                    is_sorting=0;
                    complete_sweep=-1;
                    timer_started=false;
                    total_sorting_time=0.0f;
                    comparisons=0;
                    sprintf(time_text,"Time: 0.00s | Comparisons: 0");
                }
            }

        }

        Uint32 current_ticks=SDL_GetTicks();

        Uint32 delta_ticks=current_ticks-last_time;
        last_time=current_ticks;


        //Timer Update
        if(is_sorting && i<ARRAY_SIZE){
            total_sorting_time += delta_ticks / 1000.0f;
            sprintf(time_text, "Sorting Time: %.2fs | Comparisons: %d", total_sorting_time, comparisons);
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        
        //Array sort
        if(is_sorting&&i<ARRAY_SIZE)
        {
            comparisons++;
            if(j>=0 && arr[j]>key){
                arr[j+1]=arr[j];
                j--;
            }
            else{
                arr[j+1]=key;
                i++;
                if(i<ARRAY_SIZE){
                    key=arr[i];
                    j=i-1;
                }
            }
            SDL_Delay(5);
        }
        else if(i>=ARRAY_SIZE && complete_sweep<ARRAY_SIZE){
            if(complete_sweep==-1){
                sprintf(time_text,"Final Time: %.2fs | Comparisons: %d",total_sorting_time,comparisons);

            }
            complete_sweep++;
            SDL_Delay(5);
        }
        //Render
        draw_array(renderer,arr,i,j,complete_sweep);

        if (font) render_text(renderer,font,time_text,20,20);

        SDL_RenderPresent(renderer);
    }
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}