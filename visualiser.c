#include<SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include<time.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ARRAY_SIZE 100

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
    SDL_Window* window = SDL_CreateWindow("Sorting Visualiser",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    
    int arr[ARRAY_SIZE];
    srand(time(NULL));
    for(int i=0;i<ARRAY_SIZE;i++)
    {
        arr[i] = rand()%(SCREEN_HEIGHT-50)+10;
    }

    int running=1;
    SDL_Event event;

    int i=1;
    int j=0;
    int key=arr[i];
    int is_sorting=0;
    int complete_sweep=-1;
    while(running)
    {
        while (SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running=0;
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_SPACE){
                    is_sorting=!is_sorting;
                }
                else if(event.key.keysym.sym == SDLK_ESCAPE){
                    for(int k=0;k<ARRAY_SIZE;k++){
                        arr[k]=rand()%(SCREEN_HEIGHT-50)+10;
                    }
                    i=1;
                    j=0;
                    key=arr[i];
                    is_sorting=0;
                    complete_sweep=-1;
                }
            }

        }
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        
        //Array sort
        if(is_sorting&&i<ARRAY_SIZE)
        {
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
        }
        else if(is_sorting && i>=ARRAY_SIZE && complete_sweep<ARRAY_SIZE){
            complete_sweep++;
            SDL_Delay(20);
        }
        //Render
        draw_array(renderer,arr,i,j,complete_sweep);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}