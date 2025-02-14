#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX_TAREFAS 100
#define MAX_DESC_LEN 100

Music music;
// Estrutura para armazenar as tarefas
typedef struct {
    int id;
    char descricao[MAX_DESC_LEN];
    int concluida;
} Tarefa;

Tarefa listaTarefas[MAX_TAREFAS];
int contadorTarefas = 0;
char descricaoInput[MAX_DESC_LEN] = "";  // Armazenar a descrição da tarefa em um campo de texto
bool mostrarTarefas = false;  // Controla a visibilidade da lista de tarefas


// Protótipos das funções
void adicionarTarefa();
void listarTarefas();
void salvarTarefas();
void carregarTarefas();
void desenharInterface();
void atualizarDescricaoInput();
void desenharListaTarefas();
void DrawSmoothGradient();
void marcarTarefaConcluida(int id);
void excluirTarefa();
void pausarmusica();


Color LerpColor(Color start, Color end, float t) {
    return (Color){
        (unsigned char)(start.r + t * (end.r - start.r)),
        (unsigned char)(start.g + t * (end.g - start.g)),
        (unsigned char)(start.b + t * (end.b - start.b)),
        (unsigned char)(start.a + t * (end.a - start.a))
    };
}

void DrawSmoothGradient() {
    // Defina as cores de início e fim do gradiente
    Color startColor = (Color){251, 194, 235, 255}; // #fbc2eb
    Color endColor = (Color){166, 193, 238, 255};  // #a6c1ee

    // Desenhe o gradiente na tela
    for (int i = 0; i < GetScreenHeight(); i++) {
        // Calcular a interpolação entre as cores com base na altura da tela
        float t = (float)i / GetScreenHeight(); // Normaliza a posição vertical
        Color interpolatedColor = LerpColor(startColor, endColor, t);
        
        // Desenhe a linha com a cor interpolada
        DrawRectangle(0, i, GetScreenWidth(), 1, interpolatedColor);
    }
}

// Função para adicionar uma nova tarefa
void adicionarTarefa() {
    if (contadorTarefas >= MAX_TAREFAS) {
        printf("Limite de tarefas atingido!\n");
        return;
    }

    Tarefa nova;
    nova.id = contadorTarefas + 1;

    // Garantir que a string não tenha espaços desnecessários no final
    strncpy(nova.descricao, descricaoInput, MAX_DESC_LEN - 1);
    nova.descricao[MAX_DESC_LEN - 1] = '\0';  // Garantir que a string esteja terminada

    nova.concluida = 0;
    listaTarefas[contadorTarefas++] = nova;

    salvarTarefas();

    // Limpar o campo de descrição após adicionar a tarefa
    descricaoInput[0] = '\0';
}

void pausarmusica(Music *music) {
    if (IsMusicStreamPlaying(*music)) {
        PauseMusicStream(*music);
    } else {
        ResumeMusicStream(*music);
    }
}

void excluirTarefasConcluidas() {
    int i = 0;
    while (i < contadorTarefas) {
        if (listaTarefas[i].concluida) {
            // Deslocar os elementos seguintes para preencher o espaço da tarefa removida
            for (int j = i; j < contadorTarefas - 1; j++) {
                listaTarefas[j] = listaTarefas[j + 1];
            }
            contadorTarefas--;  // Reduz o número total de tarefas
        } else {
            i++;  // Só avança se não excluir, pois os elementos mudam de posição
        }
    }
    salvarTarefas();  // Salva a lista atualizada
    printf("Tarefas concluídas excluídas com sucesso!\n");
}


// Função para salvar tarefas em um arquivo
void salvarTarefas() {
    FILE *arquivo = fopen("tarefas.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }

    for (int i = 0; i < contadorTarefas; i++) {
        // Salvar corretamente o formato no arquivo
        fprintf(arquivo, "%d;%d;%s\n", listaTarefas[i].id, listaTarefas[i].concluida, listaTarefas[i].descricao);
    }

    fclose(arquivo);
}

// Função para carregar tarefas do arquivo
void carregarTarefas() {
    FILE *arquivo = fopen("tarefas.txt", "r");
    if (arquivo == NULL) {
        return; // Arquivo não existe, então não há tarefas para carregar
    }

    contadorTarefas = 0;
    while (fscanf(arquivo, "%d;%d;%[^\n]\n", 
                  &listaTarefas[contadorTarefas].id, 
                  &listaTarefas[contadorTarefas].concluida, 
                  listaTarefas[contadorTarefas].descricao) == 3) {
        contadorTarefas++;
    }

    fclose(arquivo);
}

void marcarTarefaConcluida(int id) {
    for (int i = 0; i < contadorTarefas; i++) {
        if (listaTarefas[i].id == id) {
            listaTarefas[i].concluida = 1;  // Marcar a tarefa como concluída
            salvarTarefas();  // Salvar as alterações no arquivo
            break;
        }
    }
}


// Função para desenhar a interface
void desenharInterface() {
    ClearBackground(RAYWHITE); // Limpar o fundo
   
    Vector2 mouse = GetMousePosition();

  // Definindo a posição inicial vertical para os botões
int yPos = 150;

// Botão "Adicionar Tarefa"
Rectangle addButton = { 20, yPos, 200, 50 };  // Posição horizontal 20
if (CheckCollisionPointRec(mouse, addButton)) {
    DrawRectangleRec(addButton, LIGHTGRAY);  // Cor de fundo normal
    DrawText("Adicionar Tarefa", 35, yPos + 20, 20, DARKGRAY);
} else {
    DrawRectangleRec(addButton, DARKGRAY);  // Cor de fundo quando hover
    DrawText("Adicionar Tarefa", 35, yPos + 20, 20, LIGHTGRAY);  // Texto claro
}


// Botão "Listar Tarefas"
Rectangle listButton = { 280, yPos, 200, 50 };  // Posição horizontal 240
if (CheckCollisionPointRec(mouse, listButton)) {
    DrawRectangleRec(listButton, LIGHTGRAY);  // Cor de fundo normal
    DrawText("Listar Tarefas", 310, yPos + 20, 20, DARKGRAY);
} else {
    DrawRectangleRec(listButton, DARKGRAY);  // Cor de fundo quando hover
    DrawText("Listar Tarefas", 310, yPos + 20, 20, LIGHTGRAY);  // Texto claro
}

Image pauseImage = LoadImage("resources/pause.png");  // Carrega a imagem
ImageResize(&pauseImage, 45, 45);
Texture2D pauseTexture = LoadTextureFromImage(pauseImage); // Converte a imagem para textura
UnloadImage(pauseImage); // Não precisamos mais do `Image`, então liberamos a memória

Rectangle pausebutton = { 740, 10, 45, 45 }; // Define a área do botão

DrawTexturePro(
    pauseTexture,                  // Textura carregada
    (Rectangle){ 0, 0, pauseTexture.width, pauseTexture.height }, // Área original da textura
    pausebutton,                    // Área na tela (tamanho ajustado)
    (Vector2){ 0, 0 },               // Ponto de origem (canto superior esquerdo)
    0.0f,                            // Rotação (0°)
    WHITE                             // Cor do filtro (WHITE para manter original)
);

DrawTexture(pauseTexture, pausebutton.x, pausebutton.y, WHITE);


// Botão "Excluir Tarefa"
Rectangle deleteButton = { 530, yPos, 200, 50 };  // Posição horizontal 460
if (CheckCollisionPointRec(mouse, deleteButton)) {
    DrawRectangleRec(deleteButton, LIGHTGRAY);  // Cor de fundo normal
    DrawText("Excluir Tarefa", 560, yPos + 20, 20, DARKGRAY);
} else {
    DrawRectangleRec(deleteButton, DARKGRAY);  // Cor de fundo quando hover
    DrawText("Excluir Tarefa", 560, yPos + 20, 20, LIGHTGRAY);  // Texto claro
}

    // Desenho da caixa de entrada para a descrição da tarefa
    Rectangle textBox = { 100, 80, 600, 30 };
    DrawRectangleRec(textBox, LIGHTGRAY);
    DrawText("Descrição da tarefa:", 100, 50, 20, DARKGRAY);
    DrawText(descricaoInput, 100, 80, 20, DARKGRAY);

    // Verificar cliques nos botões
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, addButton)) {
            adicionarTarefa();  // Adicionar tarefa ao clicar no botão
        }
        if (CheckCollisionPointRec(mouse, deleteButton)) {
            excluirTarefasConcluidas();  // Excluir tarefa ao clicar no botão
        }
        if (CheckCollisionPointRec(mouse, listButton)) {
            mostrarTarefas = !mostrarTarefas; // Alternar a visibilidade das tarefas
        }
    
        if (CheckCollisionPointRec(mouse, pausebutton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            pausarmusica(&music);
        }
        
    }


    // Atualizar a descrição da tarefa a cada tecla pressionada
    atualizarDescricaoInput();
}

// Função para atualizar o texto da tarefa
void atualizarDescricaoInput() {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        int len = strlen(descricaoInput);
        if (len > 0) {
            descricaoInput[len - 1] = '\0';  // Remover o último caractere
        }
    }

    // Adicionar novas teclas pressionadas à descrição
    for (int i = 32; i < 128; i++) {  // Verificar teclas imprimíveis (32 a 127)
        if (IsKeyPressed(i)) {
            int len = strlen(descricaoInput);
            if (len < MAX_DESC_LEN - 1) {
                descricaoInput[len] = (char)i;  // Adicionar o caractere ao final
                descricaoInput[len + 1] = '\0'; // Garantir que a string esteja terminada
            }
        }
    }
}

// Função para desenhar as tarefas na interface
void desenharListaTarefas() {
    if (contadorTarefas == 0) {
        DrawText("Nenhuma tarefa cadastrada.", 50, 400, 20, DARKGRAY);
        return;
    }

     Vector2 mouse = GetMousePosition(); 

    int yPosition = 300;  // Posição inicial para desenhar as tarefas
    for (int i = 0; i < contadorTarefas; i++) {
        char tarefa[200];
        snprintf(tarefa, sizeof(tarefa), "%d. [%s] %s", listaTarefas[i].id,
                 listaTarefas[i].concluida ? "X" : " ", listaTarefas[i].descricao);
        DrawText(tarefa, 50, yPosition, 20, DARKGRAY);
        
        // Verificar se o usuário clicou sobre uma tarefa para marcar como concluída
        if (CheckCollisionPointRec(mouse, (Rectangle){ 50, yPosition, 600, 30 })) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                marcarTarefaConcluida(listaTarefas[i].id);  // Marcar como concluída
            }
        }

        yPosition += 30;  // Espaçamento entre as tarefas
    }
}

int main() {

    setlocale(LC_ALL, "Portuguese"); // Configurar a acentuação no Windows
    InitWindow(800, 600, "Bloco de Tarefas");
    InitAudioDevice();  // Inicializar o dispositivo de áudio

    Image icon = LoadImage("resources/icon.png");  // Load icon image
    SetWindowIcon(icon);                          // Set icon for window (single image, RGBA 32bit)
    UnloadImage(icon);                            // Unload icon image from memory

    music = LoadMusicStream("resources/music.mp3");
    music.looping = true;
    SetMusicVolume(music, 0.3f);

    PlayMusicStream(music);

    carregarTarefas();

    // Loop principal
    while (!WindowShouldClose()) {
        // Iniciar a musica
        UpdateMusicStream(music);
        
        // Iniciar o desenho
        BeginDrawing();
        
        DrawSmoothGradient();   // Desenha o gradiente suave no fundo
        
        desenharInterface();    // Desenha a interface gráfica

        if (mostrarTarefas) {
            desenharListaTarefas(); // Desenha a lista de tarefas apenas quando necessário
        }

        EndDrawing();  // Finaliza o desenho
    }
     
    CloseAudioDevice();
    CloseWindow(); // Fecha a janela

    return 0;
}
