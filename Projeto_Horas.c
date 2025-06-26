#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ARQUIVO_REGISTROS "horas_trabalhadas.dat"

struct RegistroDia {
    int dia;
    int entrada_h;
    int entrada_m;
    int saida_h;
    int saida_m;
    int minutos_trabalhados;
};

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void cabecalho() {
    printf("\n=======================================\n");
    printf("   CONTROLE DE HORAS MENSALISTA 4.0\n");
    printf("=======================================\n");
}

int calcularMinutosTrabalhados(int entrada_h, int entrada_m, int saida_h, int saida_m) {
    int minutos_entrada = entrada_h * 60 + entrada_m;
    int minutos_saida = saida_h * 60 + saida_m;
    
    if(minutos_saida < minutos_entrada) {
        return (24*60 - minutos_entrada) + minutos_saida;
    }
    return minutos_saida - minutos_entrada;
}

void formatarHora(int minutos, char *buffer) {
    int horas = minutos / 60;
    int min = minutos % 60;
    sprintf(buffer, "%02d:%02d", horas, min);
}

void adicionarRegistro(struct RegistroDia registros[], int *contador, int total_dias) {
    if(*contador >= total_dias) {
        printf("\nTodos os dias ja foram registrados! (%d/%d)\n", *contador, total_dias);
        return;
    }

    cabecalho();
    printf("\n>>> ADICIONAR DIA TRABALHADO [%d/%d]\n", *contador + 1, total_dias);
    
    struct RegistroDia novo;
    novo.dia = *contador + 1;
    
    // Entrada
    printf("\nHORARIO DE ENTRADA:\n");
    printf("  Hora (00-23): ");
    scanf("%d", &novo.entrada_h);
    printf("  Minutos (00-59): ");
    scanf("%d", &novo.entrada_m);
    
    // Saída
    printf("\nHORARIO DE SAIDA:\n");
    printf("  Hora (00-23): ");
    scanf("%d", &novo.saida_h);
    printf("  Minutos (00-59): ");
    scanf("%d", &novo.saida_m);
    
    // Validação
    if(novo.entrada_h < 0 || novo.entrada_h > 23 ||
       novo.saida_h < 0 || novo.saida_h > 23 ||
       novo.entrada_m < 0 || novo.entrada_m > 59 ||
       novo.saida_m < 0 || novo.saida_m > 59) {
        printf("\nERRO: Valores inválidos! Registro cancelado.\n");
        return;
    }
    
    // Cálculo
    novo.minutos_trabalhados = calcularMinutosTrabalhados(
        novo.entrada_h, novo.entrada_m,
        novo.saida_h, novo.saida_m
    );
    
    registros[*contador] = novo;
    (*contador)++;
    
    printf("\nRegistro adicionado com sucesso! (%d/%d)\n", *contador, total_dias);
}

void listarRegistros(struct RegistroDia registros[], int contador, int total_dias) {
    cabecalho();
    
    if(contador == 0) {
        printf("\nNenhum registro encontrado!\n");
        return;
    }
    
    printf("\nLISTA DE REGISTROS (%d de %d dias):\n", contador, total_dias);
    printf("-------------------------------------------------------------------\n");
    printf(" Dia | Entrada | Saida  | Total  | Minutos | Status\n");
    printf("-------------------------------------------------------------------\n");
    
    char buffer[10];
    for(int i = 0; i < contador; i++) {
        printf(" %2d  |", registros[i].dia);
        
        // Entrada
        formatarHora(registros[i].entrada_h * 60 + registros[i].entrada_m, buffer);
        printf(" %5s  |", buffer);
        
        // Saída
        formatarHora(registros[i].saida_h * 60 + registros[i].saida_m, buffer);
        printf(" %5s  |", buffer);
        
        // Total trabalhado
        formatarHora(registros[i].minutos_trabalhados, buffer);
        printf(" %5s  |", buffer);
        
        // Minutos
        printf(" %6d  |", registros[i].minutos_trabalhados);
        
        // Status (comparando com 6 horas = 360 minutos)
        int min_esperados = 360;
        int diferenca = registros[i].minutos_trabalhados - min_esperados;
        
        if(diferenca >= 0) {
            printf(" +%d min\n", diferenca);
        } else {
            printf(" %d min\n", diferenca);
        }
    }
    printf("-------------------------------------------------------------------\n");
    
    // Legenda de status
    printf("\nLegenda: Diferença em relação a 6 horas (360 minutos) por dia\n");
}

void relatorioMensal(struct RegistroDia registros[], int contador, int total_dias) {
    cabecalho();
    
    if(contador == 0) {
        printf("\nNenhum registro encontrado!\n");
        return;
    }
    
    int total_minutos = 0;
    for(int i = 0; i < contador; i++) {
        total_minutos += registros[i].minutos_trabalhados;
    }
    
    // Cálculos para horas trabalhadas
    int total_horas = total_minutos / 60;
    int total_minutos_res = total_minutos % 60;
    
    // Cálculos para horas esperadas (6 horas por dia)
    int minutos_esperados = total_dias * 360; // 6 horas = 360 minutos
    int horas_esperadas = minutos_esperados / 60;
    int minutos_esperados_res = minutos_esperados % 60;
    
    // Cálculo da diferença
    int diferenca_minutos = total_minutos - minutos_esperados;
    char sinal = diferenca_minutos >= 0 ? '+' : '-';
    int abs_diferenca = abs(diferenca_minutos);
    int horas_diferenca = abs_diferenca / 60;
    int minutos_diferenca = abs_diferenca % 60;
    
    // Determinar status
    char *status;
    if(diferenca_minutos > 0) {
        status = "EXCEDENTE";
    } else if(diferenca_minutos < 0) {
        status = "DEFICIT";
    } else {
        status = "QUITE";
    }

    printf("\nRELATORIO MENSAL:\n");
    printf("========================================================================\n");
    printf(" Dias registrados: %d de %d\n", contador, total_dias);
    printf("------------------------------------------------------------------------\n");
    printf(" Horas trabalhadas:   %6d:%02d  (%8d minutos)\n", total_horas, total_minutos_res, total_minutos);
    printf(" Horas esperadas:     %6d:%02d  (%8d minutos)\n", horas_esperadas, minutos_esperados_res, minutos_esperados);
    printf("------------------------------------------------------------------------\n");
    printf(" Diferenca:           %c%02d:%02d  (%8d minutos)\n", 
           sinal, horas_diferenca, minutos_diferenca, diferenca_minutos);
    printf(" Status:              %s\n", status);
    printf("========================================================================\n");
    
    if(contador < total_dias) {
        int dias_faltantes = total_dias - contador;
        printf("\n ATENCAO: Faltam %d dias para registrar!\n", dias_faltantes);
    }
}

int salvarRegistros(struct RegistroDia registros[], int contador, int total_dias) {
    FILE *arquivo = fopen(ARQUIVO_REGISTROS, "wb");
    if(arquivo == NULL) {
        return 0; // Falha ao abrir arquivo
    }
    
    // Salva o cabeçalho com total_dias e contador
    fwrite(&total_dias, sizeof(int), 1, arquivo);
    fwrite(&contador, sizeof(int), 1, arquivo);
    
    // Salva todos os registros
    fwrite(registros, sizeof(struct RegistroDia), contador, arquivo);
    
    fclose(arquivo);
    return 1; // Sucesso
}

int carregarRegistros(struct RegistroDia **registros, int *contador, int *total_dias) {
    FILE *arquivo = fopen(ARQUIVO_REGISTROS, "rb");
    if(arquivo == NULL) {
        return 0; // Arquivo não existe
    }
    
    // Carrega o cabeçalho com total_dias e contador
    fread(total_dias, sizeof(int), 1, arquivo);
    fread(contador, sizeof(int), 1, arquivo);
    
    // Aloca memória para os registros
    *registros = malloc(*total_dias * sizeof(struct RegistroDia));
    if(*registros == NULL) {
        fclose(arquivo);
        return 0; // Falha na alocação
    }
    
    // Carrega os registros
    fread(*registros, sizeof(struct RegistroDia), *contador, arquivo);
    
    fclose(arquivo);
    return 1; // Sucesso
}

int menuPrincipal(int contador, int total_dias) {
    cabecalho();
    printf("\nMENU PRINCIPAL:\n");
    printf(" Dias registrados: %d/%d\n", contador, total_dias);
    printf(" Jornada diaria esperada: 6 horas\n\n");
    
    printf(" 1. Adicionar dia trabalhado\n");
    printf(" 2. Listar todos os registros\n");
    printf(" 3. Ver relatorio mensal\n");
    printf(" 4. Sair do programa\n");
    
    if(contador >= total_dias) {
        printf("\n TODOS OS DIAS FORAM REGISTRADOS!\n");
    }
    
    printf("\nEscolha uma opcao: ");
    
    int opcao;
    scanf("%d", &opcao);
    return opcao;
}

int main() {
    cabecalho();
    
    struct RegistroDia *registros = NULL;
    int contador = 0;
    int total_dias = 0;
    int executando = 1;
    
    // Tenta carregar dados existentes
    if(carregarRegistros(&registros, &contador, &total_dias)) {
        printf("\nDados carregados com sucesso! (%d dias registrados)\n", contador);
    } else {
        printf("\nNenhum dado encontrado ou erro ao carregar.\n");
        
        // Definir quantidade de dias para registrar
        while(total_dias <= 0 || total_dias > 31) {
            printf("\nQuantos dias trabalhados deseja registrar? (1-31): ");
            scanf("%d", &total_dias);
            
            if(total_dias <= 0 || total_dias > 31) {
                printf("Valor invalido! Digite um numero entre 1 e 31.\n");
            }
        }
        
        registros = malloc(total_dias * sizeof(struct RegistroDia));
        if(registros == NULL) {
            printf("Erro ao alocar memoria! Encerrando...\n");
            return 1;
        }
    }
    
    while(executando) {
        int opcao = menuPrincipal(contador, total_dias);
        limparBuffer(); // Limpa o buffer do teclado
        
        switch(opcao) {
            case 1:
                adicionarRegistro(registros, &contador, total_dias);
                break;
            case 2:
                listarRegistros(registros, contador, total_dias);
                break;
            case 3:
                relatorioMensal(registros, contador, total_dias);
                break;
            case 4:
                executando = 0;
                printf("\nSalvando dados... ");
                if(salvarRegistros(registros, contador, total_dias)) {
                    printf("Dados salvos com sucesso!\n");
                } else {
                    printf("Erro ao salvar dados!\n");
                }
                printf("Encerrando programa... Obrigado!\n\n");
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
        }
        
        if(executando && opcao != 4) {
            printf("\nPressione Enter para continuar...");
            limparBuffer();
            system("clear || cls"); // Limpa a tela
        }
    }
    
    free(registros); // Libera memória alocada
    return 0;
}