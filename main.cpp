#include "raylib.h"
#include "game.h"

int main(){
    int screenWidth = 800;
    int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Lochy");
    SetExitKey(0);
    SetTargetFPS(30);

    // Inicjalizacja systemu audio i wczytanie muzyki
    InitAudioDevice();
    Music bgMusic = LoadMusicStream("muzyka.mp3");
    PlayMusicStream(bgMusic); // Rozpoczęcie odtwarzania

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
        
        // Aktualizacja strumienia audio (niezbędne, aby muzyka grała płynnie)
        UpdateMusicStream(bgMusic);

        // obsługa przycisku wyjscia wewnątrz logiki okna
        if (game.isPaused || game.isGameOver) {
            Vector2 mousePos = GetMousePosition();
            Rectangle btnQuit = { (float)screenWidth/2 - 100, (float)screenHeight/2 + 10, 200, 50 };
            Rectangle btnResume = { (float)screenWidth/2 - 100, (float)screenHeight/2 - 60, 200, 50 };
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, btnQuit)) {
                    UnloadMusicStream(bgMusic);
                    CloseAudioDevice();
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
    
    // Sprzątanie po zamknięciu gry
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();

    return 0;
}