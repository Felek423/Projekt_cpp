#include "raylib.h"
#include "game.h"

int main(){
    int screenWidth = 800;
    int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Lochy");
    SetExitKey(0);
    SetTargetFPS(30);

    screenWidth = GetMonitorWidth(GetCurrentMonitor());
    screenHeight = GetMonitorHeight(GetCurrentMonitor());    
    ToggleFullscreen();

    Game game(screenWidth, screenHeight);

    while(!WindowShouldClose()){
        // obsługa pauzy
        if(IsKeyPressed(KEY_ESCAPE)){
            game.isPaused = !game.isPaused;
        }

        // aktualizacja matematyki gry, jesli gracz nie zginął
        if(!game.isPaused && !game.isGameOver){
            game.Update();
        }
        
        // obsługa przycisku wyjscia wewnątrz logiki okna
        if (game.isPaused || game.isGameOver) {
            Vector2 mousePos = GetMousePosition();
            Rectangle btnQuit = { (float)screenWidth/2 - 100, (float)screenHeight/2 + 10, 200, 50 };
            Rectangle btnResume = { (float)screenWidth/2 - 100, (float)screenHeight/2 - 60, 200, 50 };
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, btnQuit)) {
                    CloseWindow();
                    return 0;      
                }
                if (game.isPaused && CheckCollisionPointRec(mousePos, btnResume)) {
                    game.isPaused = false;
                }
            }
        }

        BeginDrawing();
        ClearBackground(GRAY);
        
        // funkcja odpowiadająca za rysowanie na podzielone ekrany
        game.Draw();
        
        EndDrawing();
    } 
    return 0;
}