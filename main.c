#include "raylib.h"
#include <emscripten.h>
#include <stdio.h>
#include <string.h>

// Variables globales
char nombreUsuario[64] = "";
bool enPantallaInicio = true;
float resultadoHuella = 12.5f; // Valor de prueba/calculado

// Función expuesta para recibir el nombre desde JS
EMSCRIPTEN_KEEPALIVE
void ActualizarNombreDesdeJS(const char* nombre) {
    snprintf(nombreUsuario, sizeof(nombreUsuario), "%s", nombre);
}

// Función principal de bucle (Main Loop)
void UpdateDrawFrame(void) {
    // Lógica
    if (enPantallaInicio) {
        // Detectar si el usuario hace clic o presiona ENTER para iniciar
        if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseY() > 300)) {
            enPantallaInicio = false;
            
            // Llama a JS para ocultar el cuadro de texto HTML
            emscripten_run_script("OcultarInputNombre();");

            // Envía los datos calculados a Google Sheets
            char scriptBuffer[256];
            snprintf(scriptBuffer, sizeof(scriptBuffer), "EnviarAGoogleSheets('%s', %.2f);", 
                     strlen(nombreUsuario) > 0 ? nombreUsuario : "Anonimo", resultadoHuella);
            emscripten_run_script(scriptBuffer);
        }
    }

    // Dibujado en Raylib
    BeginDrawing();
        ClearBackground(RAYWHITE);

        if (enPantallaInicio) {
            DrawText("Calculadora de Huella de Carbono", 80, 50, 22, DARKGREEN);
            
            if (strlen(nombreUsuario) > 0) {
                DrawText(TextFormat("Bienvenido: %s", nombreUsuario), 100, 240, 20, BLUE);
            }

            // Dibujar botón de comenzar
            DrawRectangle(100, 320, 200, 50, LIME);
            DrawText("COMENZAR", 140, 335, 20, DARKGREEN);
        } else {
            DrawText("¡Calculando tu Huella de Carbono!", 60, 100, 20, DARKBLUE);
            DrawText(TextFormat("Usuario: %s", nombreUsuario), 60, 150, 18, BLACK);
            DrawText(TextFormat("Resultado: %.2f kg CO2", resultadoHuella), 60, 190, 18, RED);
            DrawText("Datos enviados a Google Sheets correctamente.", 60, 250, 14, GRAY);
        }

    EndDrawing();
}

int main(void) {
    // Configuración de pantalla fija óptima para simulación responsiva
    InitWindow(450, 600, "Calculadora Huella de Carbono");
    SetTargetFPS(60);

    // Bucle principal de Emscripten para la web
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);

    CloseWindow();
    return 0;
}