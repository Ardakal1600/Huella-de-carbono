#include "raylib.h"
#include <stdio.h>
#include <string.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

typedef struct {
    int transporte;
    int tiempo;
    int electricidad;
    int carne;
    int gas;
    int reciclaje;
} Respuestas;

int pasoActual = 0;
Respuestas respuestas = {0};
char nombreUsuario[64] = "Usuario";
float huellaTotal = 0.0f;
bool enviadoASheets = false;

#if defined(PLATFORM_WEB)
EMSCRIPTEN_KEEPALIVE
#endif
void ActualizarNombreDesdeJS(const char* nuevoNombre) {
    if (nuevoNombre && strlen(nuevoNombre) > 0) {
        snprintf(nombreUsuario, sizeof(nombreUsuario), "%s", nuevoNombre);
    } else {
        snprintf(nombreUsuario, sizeof(nombreUsuario), "Anonimo");
    }
}

void CalcularHuella(void) {
    huellaTotal = 0.0f;
    if (respuestas.transporte == 1) huellaTotal += 0.1f;
    else if (respuestas.transporte == 2) huellaTotal += 0.8f;
    else if (respuestas.transporte == 3) huellaTotal += 2.5f;

    if (respuestas.tiempo == 1) huellaTotal += 0.2f;
    else if (respuestas.tiempo == 2) huellaTotal += 0.5f;
    else if (respuestas.tiempo == 3) huellaTotal += 1.2f;

    if (respuestas.electricidad == 1) huellaTotal += 0.5f;
    else if (respuestas.electricidad == 2) huellaTotal += 1.2f;
    else if (respuestas.electricidad == 3) huellaTotal += 2.5f;

    if (respuestas.carne == 1) huellaTotal += 0.4f;
    else if (respuestas.carne == 2) huellaTotal += 1.1f;
    else if (respuestas.carne == 3) huellaTotal += 2.2f;

    if (respuestas.gas == 1) huellaTotal += 0.3f;
    else if (respuestas.gas == 2) huellaTotal += 0.8f;
    else if (respuestas.gas == 3) huellaTotal += 1.5f;

    if (respuestas.reciclaje == 1) huellaTotal -= 0.3f;
    else if (respuestas.reciclaje == 2) huellaTotal += 0.0f;
    else if (respuestas.reciclaje == 3) huellaTotal += 0.5f;

    if (huellaTotal < 0.1f) huellaTotal = 0.1f;
}

void SiguientePaso(void) {
    pasoActual++;
    if (pasoActual > 6) {
        CalcularHuella();
        pasoActual = 7;
    }
}

void PasoAnterior(void) {
    if (pasoActual > 1) pasoActual--;
}

void EnviarDatosAGoogleSheets(void) {
#if defined(PLATFORM_WEB)
    emscripten_run_script(TextFormat("window.EnviarAGoogleSheets('%s', %.2f);", nombreUsuario, huellaTotal));
#endif
    enviadoASheets = true;
}

bool DibujarBoton(Rectangle rec, const char* texto, Color colorFondo, Color colorTexto) {
    Vector2 mousePos = GetMousePosition();
    bool estaEncima = CheckCollisionPointRec(mousePos, rec);
    
    Color colActual = estaEncima ? LIGHTGRAY : colorFondo;
    DrawRectangleRec(rec, colActual);
    DrawRectangleLinesEx(rec, 2, GRAY);

    int fontSize = 18;
    int textWidth = MeasureText(texto, fontSize);
    int posX = rec.x + (rec.width - textWidth) / 2;
    int posY = rec.y + (rec.height - fontSize) / 2;

    DrawText(texto, posX, posY, fontSize, colorTexto);

    return (estaEncima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

int main(void) {
    InitWindow(800, 600, "Calculadora de Huella de Carbono");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRounded((Rectangle){20, 20, 760, 560}, 0.05f, 4, WHITE);
        DrawRectangleRoundedLines((Rectangle){20, 20, 760, 560}, 0.05f, 4, DARKGRAY);

        Color textoColor = DARKGRAY;

        if (pasoActual == 0) {
            enviadoASheets = false;

            DrawText("Calculadora de Huella de Carbono", 180, 80, 26, GREEN);
            DrawText("Ingresa tu nombre arriba y haz clic en comenzar:", 50, 160, 18, textoColor);
            
            if (DibujarBoton((Rectangle){250, 320, 300, 50}, "Comenzar Encuesta", LIGHTGRAY, BLACK)) {
#if defined(PLATFORM_WEB)
                emscripten_run_script("window.ocultarInputNombre();");
#endif
                SiguientePaso();
            }
        }
        else if (pasoActual >= 1 && pasoActual <= 6) {
            DrawText(TextFormat("Pregunta %d de 6", pasoActual), 50, 50, 20, DARKGREEN);

            if (pasoActual > 1) {
                if (DibujarBoton((Rectangle){650, 40, 110, 35}, "<- Atrás", LIGHTGRAY, BLACK)) {
                    PasoAnterior();
                }
            }

            Rectangle btn1 = {50, 160, 700, 50};
            Rectangle btn2 = {50, 230, 700, 50};
            Rectangle btn3 = {50, 300, 700, 50};

            switch (pasoActual) {
                case 1:
                    DrawText("¿Qué medio de transporte usas con más frecuencia?", 50, 100, 20, textoColor);
                    if (DibujarBoton(btn1, "1. A pie / Bicicleta", WHITE, BLACK)) { respuestas.transporte = 1; SiguientePaso(); }
                    if (DibujarBoton(btn2, "2. Transporte Público (Bus / Metro)", WHITE, BLACK)) { respuestas.transporte = 2; SiguientePaso(); }
                    if (DibujarBoton(btn3, "3. Vehículo Particular / Moto", WHITE, BLACK)) { respuestas.transporte = 3; SiguientePaso(); }
                    break;
                case 2:
                    DrawText("¿Cuánto tiempo dedicas diariamente al transporte?", 50, 100, 20, textoColor);
                    if (DibujarBoton(btn1, "1. Menos de 30 minutos", WHITE, BLACK)) { respuestas.tiempo = 1; SiguientePaso(); }
                    if (DibujarBoton(btn2, "2. Entre 30 y 60 minutos", WHITE, BLACK)) { respuestas.tiempo = 2; SiguientePaso(); }
                    if (DibujarBoton(btn3, "3. Más de 60 minutos", WHITE, BLACK)) { respuestas.tiempo = 3; SiguientePaso(); }
                    break;
                case 3:
                    DrawText("¿A cuánto asciende tu consumo mensual de electricidad?", 50, 100, 20, textoColor);
                    if (DibujarBoton(btn1, "1. Menos de $30", WHITE, BLACK)) { respuestas.electricidad = 1; SiguientePaso(); }
                    if (DibujarBoton(btn2, "2. Entre $30 y $60", WHITE, BLACK)) { respuestas.electricidad = 2; SiguientePaso(); }
                    if (DibujarBoton(btn3, "3. Más de $60", WHITE, BLACK)) { respuestas.electricidad = 3; SiguientePaso(); }
                    break;
                case 4:
                    DrawText("¿Con qué frecuencia consumes carne roja a la semana?", 50, 100, 20, textoColor);
                    if (DibujarBoton(btn1, "1. Rara vez / Vegetariano", WHITE, BLACK)) { respuestas.carne = 1; SiguientePaso(); }
                    if (DibujarBoton(btn2, "2. 1 a 3 veces por semana", WHITE, BLACK)) { respuestas.carne = 2; SiguientePaso(); }
                    if (DibujarBoton(btn3, "3. Casi todos los días", WHITE, BLACK)) { respuestas.carne = 3; SiguientePaso(); }
                    break;
                case 5:
                    DrawText("¿Cuántos tanques/cilindros de gas usas al mes?", 50, 100, 20, textoColor);
                    if (DibujarBoton(btn1, "1. Menos de 1 tanque", WHITE, BLACK)) { respuestas.gas = 1; SiguientePaso(); }
                    if (DibujarBoton(btn2, "2. 1 tanque completo", WHITE, BLACK)) { respuestas.gas = 2; SiguientePaso(); }
                    if (DibujarBoton(btn3, "3. Más de 1 tanque", WHITE, BLACK)) { respuestas.gas = 3; SiguientePaso(); }
                    break;
                case 6:
                    DrawText("¿Separaste o reciclaste residuos en el último mes?", 50, 100, 20, textoColor);
                    if (DibujarBoton(btn1, "1. Siempre", WHITE, BLACK)) { respuestas.reciclaje = 1; SiguientePaso(); }
                    if (DibujarBoton(btn2, "2. Ocasionalmente", WHITE, BLACK)) { respuestas.reciclaje = 2; SiguientePaso(); }
                    if (DibujarBoton(btn3, "3. Casi nunca", WHITE, BLACK)) { respuestas.reciclaje = 3; SiguientePaso(); }
                    break;
            }
        }
        else if (pasoActual == 7) {
            DrawText("Resultado de tu Huella de Carbono", 180, 60, 24, GREEN);
            DrawText(TextFormat("Usuario: %s", nombreUsuario), 50, 120, 20, DARKGRAY);
            DrawText(TextFormat("Huella estimada: %.2f Toneladas CO2/año", huellaTotal), 50, 160, 22, RED);

            if (huellaTotal < 3.0f) {
                DrawText("¡Excelente! Tu impacto ambiental es bajo.", 50, 210, 18, DARKGREEN);
            } else if (huellaTotal <= 6.0f) {
                DrawText("Tu impacto es moderado. Hay margen para mejorar.", 50, 210, 18, ORANGE);
            } else {
                DrawText("Tu impacto es alto. Considera reducir el consumo de gas/auto.", 50, 210, 18, RED);
            }

            if (!enviadoASheets) {
                if (DibujarBoton((Rectangle){220, 320, 360, 50}, "Enviar Resultados a Google Sheets", LIGHTGRAY, BLACK)) {
                    EnviarDatosAGoogleSheets();
                }
            } else {
                DrawText("¡Resultados enviados con éxito!", 260, 335, 18, DARKGREEN);
            }

            if (DibujarBoton((Rectangle){250, 420, 300, 50}, "Reiniciar Test", LIGHTGRAY, BLACK)) {
#if defined(PLATFORM_WEB)
                emscripten_run_script("window.mostrarInputNombre();");
#endif
                pasoActual = 0;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}