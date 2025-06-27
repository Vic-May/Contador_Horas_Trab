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
    printf("   CONTROLE DE HORAS MENSALISTA 4.5\n");
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

void adicionarRegistro(struct RegistroDia **registros, int *contador, int *capacidade) {
    if(*contador >= *capacidade) {
        // Aumenta a capacidade em blocos de 5 dias
        *capacidade += 5;
        *registros = realloc(*registros, *capacidade * sizeof(struct RegistroDia));
        printf("\nCapacidade aumentada para %d dias\n", *capacidade);
    }

    cabecalho();
    printf("\n>>> ADICIONAR DIA TRABALHADO [%d]\n", *contador + 1);
    
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
    
    (*registros)[*contador] = novo;
    (*contador)++;
    
    printf("\nRegistro adicionado com sucesso! (Total: %d dias)\n", *contador);
}

void listarRegistros(struct RegistroDia *registros, int contador) {
    cabecalho();
    
    if(contador == 0) {
        printf("\nNenhum registro encontrado!\n");
        return;
    }
    
    printf("\nLISTA DE REGISTROS (%d dias):\n", contador);
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
    printf("\nLegenda: Diferenca em relacao a 6 horas (360 minutos) por dia\n");
}

void apagarRegistro(struct RegistroDia **registros, int *contador) {
    if(*contador == 0) {
        printf("\nNenhum registro para apagar!\n");
        return;
    }
    
    cabecalho();
    printf("\n>>> APAGAR REGISTRO DE DIA\n");
    
    // Mostra lista simplificada de dias
    printf("\nDias registrados: ");
    for(int i = 0; i < *contador; i++) {
        printf("%d", (*registros)[i].dia);
        if(i < *contador - 1) printf(", ");
    }
    printf("\n");
    
    int dia_apagar;
    printf("\nDigite o numero do dia que deseja apagar: ");
    scanf("%d", &dia_apagar);
    
    // Encontra o índice do dia
    int indice = -1;
    for(int i = 0; i < *contador; i++) {
        if((*registros)[i].dia == dia_apagar) {
            indice = i;
            break;
        }
    }
    
    if(indice == -1) {
        printf("\nDia %d não encontrado!\n", dia_apagar);
        return;
    }
    
    // Confirmação
    char confirmacao;
    printf("\nTem certeza que deseja apagar o dia %d? (S/N): ", dia_apagar);
    scanf(" %c", &confirmacao);
    
    if(toupper(confirmacao) != 'S') {
        printf("\nOperação cancelada!\n");
        return;
    }
    
    // Remove o registro deslocando os demais
    for(int i = indice; i < *contador - 1; i++) {
        (*registros)[i] = (*registros)[i+1];
        (*registros)[i].dia = i + 1; // Atualiza o número do dia
    }
    
    (*contador)--;
    
    // Atualiza os números dos dias restantes
    for(int i = indice; i < *contador; i++) {
        (*registros)[i].dia = i + 1;
    }
    
    printf("\nDia %d apagado com sucesso! (Total restante: %d dias)\n", dia_apagar, *contador);
}

void relatorioMensal(struct RegistroDia *registros, int contador) {
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
    
    // Cálculos para horas esperadas (6 horas por dia trabalhado)
    int minutos_esperados = contador * 360; // 6 horas = 360 minutos
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
    printf(" Dias trabalhados: %d\n", contador);
    printf("------------------------------------------------------------------------\n");
    printf(" Horas trabalhadas:   %6d:%02d  (%8d minutos)\n", total_horas, total_minutos_res, total_minutos);
    printf(" Horas esperadas:     %6d:%02d  (%8d minutos)\n", horas_esperadas, minutos_esperados_res, minutos_esperados);
    printf("------------------------------------------------------------------------\n");
    printf(" Diferenca:           %c%02d:%02d  (%8d minutos)\n", 
           sinal, horas_diferenca, minutos_diferenca, diferenca_minutos);
    printf(" Status:              %s\n", status);
    printf("========================================================================\n");
}

int salvarRegistros(struct RegistroDia *registros, int contador) {
    FILE *arquivo = fopen(ARQUIVO_REGISTROS, "wb");
    if(arquivo == NULL) {
        return 0; // Falha ao abrir arquivo
    }
    
    // Salva o número de registros
    fwrite(&contador, sizeof(int), 1, arquivo);
    
    // Salva todos os registros
    fwrite(registros, sizeof(struct RegistroDia), contador, arquivo);
    
    fclose(arquivo);
    return 1; // Sucesso
}

int carregarRegistros(struct RegistroDia **registros, int *contador, int *capacidade) {
    FILE *arquivo = fopen(ARQUIVO_REGISTROS, "rb");
    if(arquivo == NULL) {
        return 0; // Arquivo não existe
    }
    
    // Carrega o número de registros
    fread(contador, sizeof(int), 1, arquivo);
    
    // Define capacidade inicial (mínimo 5, ou mais se necessário)
    *capacidade = (*contador < 5) ? 5 : *contador + 5;
    *registros = malloc(*capacidade * sizeof(struct RegistroDia));
    if(*registros == NULL) {
        fclose(arquivo);
        return 0; // Falha na alocação
    }
    
    // Carrega os registros
    fread(*registros, sizeof(struct RegistroDia), *contador, arquivo);
    
    fclose(arquivo);
    return 1; // Sucesso
}

int menuPrincipal(int contador) {
    cabecalho();
    printf("\nMENU PRINCIPAL:\n");
    printf(" Dias registrados: %d\n", contador);
    printf(" Jornada diaria esperada: 6 horas\n\n");
    
    printf(" 1. Adicionar dia trabalhado\n");
    printf(" 2. Listar todos os registros\n");
    printf(" 3. Apagar dia especifico\n");
    printf(" 4. Ver relatorio mensal\n");
    printf(" 5. Sair do programa\n");
    
    int opcao;
    scanf("%d", &opcao);
    return opcao;
}

int main() {
    cabecalho();
    
    struct RegistroDia *registros = NULL;
    int contador = 0;
    int capacidade = 5; // Capacidade inicial
    int executando = 1;
    
    // Tenta carregar dados existentes
    if(carregarRegistros(&registros, &contador, &capacidade)) {
        printf("\nDados carregados com sucesso! (%d dias registrados)\n", contador);
    } else {
        printf("\nNenhum dado encontrado. Iniciando novo registro.\n");
        // Aloca memória inicial
        registros = malloc(capacidade * sizeof(struct RegistroDia));
        if(registros == NULL) {
            printf("Erro ao alocar memoria! Encerrando...\n");
            return 1;
        }
    }
    
    while(executando) {
        int opcao = menuPrincipal(contador);
        limparBuffer(); // Limpa o buffer do teclado
        
        switch(opcao) {
            case 1:
                adicionarRegistro(&registros, &contador, &capacidade);
                break;
            case 2:
                listarRegistros(registros, contador);
                break;
            case 3:
                apagarRegistro(&registros, &contador);
                break;
            case 4:
                relatorioMensal(registros, contador);
                break;
            case 5:
                executando = 0;
                printf("\nSalvando dados... ");
                if(salvarRegistros(registros, contador)) {
                    printf("Dados salvos com sucesso!\n");
                } else {
                    printf("Erro ao salvar dados!\n");
                }
                printf("Encerrando programa... Obrigado!\n\n");
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
        }
        
        if(executando && opcao != 5) {
            printf("\nPressione Enter para continuar...");
            limparBuffer();
            system("clear || cls"); // Limpa a tela
        }
    }
    
    free(registros); // Libera memória alocada
    return 0;
}