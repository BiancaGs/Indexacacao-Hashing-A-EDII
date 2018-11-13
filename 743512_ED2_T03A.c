/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA: 743512
 * Aluno: Bianca Gomes Rodrigues
 * ========================================================================== */

 /* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO 			 			"**********************EXCLUIR*********************\n"

#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA 		"ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"


/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;

/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
  } Hashtable;

  /* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */


/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);

/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
Produto Recuperar_Registro(int RRN);

void gerarChave(Produto * Novo);

void Criar_Tabela(Hashtable *Tabela, int Tamanho);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() 
{    
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);
	printf("Tamanho da Tabela Hash %d\n", tam);

	Hashtable tabela;
	Criar_Tabela(&tabela, tam);
	// if (carregarArquivo) 
	// 	carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
			scanf("%d%*c", &opcao);
			switch(opcao) 
			{
				case 1:
					cadastrar(&tabela);
					break;
				case 2:
					printf(INICIO_ALTERACAO);
					// if(alterar(tabela))
					// 	printf(SUCESSO);
					// else
					// 	printf(FALHA);
					break;
				case 3:
					printf(INICIO_BUSCA);
					// buscar(tabela);
					break;
				case 4:
					printf(INICIO_EXCLUSAO);
					// if(	remover(&tabela))
					// 	printf(SUCESSO);
					// else
					// 	printf(FALHA);
					break;
				case 5:
					printf(INICIO_LISTAGEM);
					// imprimir_tabela(tabela);
					break;
				case 6:
					// liberar_tabela(&tabela);
					break;
				case 10:
					printf(INICIO_ARQUIVO);
					printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
					break;
				default:
					printf(OPCAO_INVALIDA);
					break;
			}
	}
	return 0;
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/*Auxiliar para a função de hash*/
short f(char x){
	return (x < 59) ? x - 48 : x - 54; 
}

/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = Recuperar_Registro(rrn);
  	char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strncpy(categorias, j.categoria, strlen(j.categoria));
  	
	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");
	
	return 1;
}

/* ----------------------------------------*/
int  prox_primo(int a){

	int Contador = 0;
	for(int i=1; i <= a; i++){
		if(a%i == 0)
			Contador++;
	}
	if(Contador == 2)
		return a;
	
	else{
		int flag = 0;
		while(flag == 0){
			
			Contador = 0;
			a++;
		
			for(int j=1; j <= a; j++){
				if(a%j == 0)
					Contador++;
			}

			if(Contador == 2){
				flag = 1;
				return a;
			}	
		}				
	}
}

/*Cria e inicializa a TABELA com o TAMANHO inserido pelo usuário*/
void Criar_Tabela(Hashtable *Tabela, int Tamanho){

	Tabela->tam = Tamanho;
	Tabela->v = (Chave*)malloc(Tamanho*sizeof(Chave));
	
	for(int i = 0; i < Tamanho; i++){
		/*Estado Inicial - Livre*/
		Tabela->v[i].estado = 0;

		Tabela->v[i].rrn = -1;
	}
}

Produto Recuperar_Registro(int RRN){
	
	char Registro[193], *p;
	strncpy(Registro, ARQUIVO + ((RRN)*192), 192);
	Registro[192] = '\0';

	Produto A;
	p = strtok(Registro,"@");
	strcpy(A.pk,p);
	p = strtok(NULL,"@");
	strcpy(A.nome,p);
	p = strtok(NULL,"@");
	strcpy(A.marca,p);
	p = strtok(NULL,"@");
	strcpy(A.data,p);
	p = strtok(NULL,"@");
	strcpy(A.ano,p);
	p = strtok(NULL,"@");
	strcpy(A.preco,p);
	p = strtok(NULL,"@");
	strcpy(A.desconto,p);
	p = strtok(NULL,"@");
	strcpy(A.categoria,p);
	
	return A;

}

void gerarChave(Produto * Novo){

	Novo->pk[0] = Novo->nome[0];
	Novo->pk[1] = Novo->nome[1];
	Novo->pk[2] = Novo->marca[0];
	Novo->pk[3] = Novo->marca[1];
	Novo->pk[4] = Novo->data[0];
	Novo->pk[5] = Novo->data[1];
	Novo->pk[6] = Novo->data[3];
	Novo->pk[7] = Novo->data[4];
	Novo->pk[8] = Novo->ano[0];
	Novo->pk[9] = Novo->ano[1]; 
	Novo->pk[10] = '\0';

}

void cadastrar(Hashtable* tabela){

	//Código - NÃO é inserido pelo usuário 
	// char pk[TAM_PRIMARY_KEY];
	// gerarChave(novo);

	/*-----------------------*/

	/* Interação com o Usuário */
	Produto Novo;
	/* CAMPOS DE TAMANHO VARIÁVEL */
	
	//Nome do Produto ou Modelo
	// char Nome[TAM_NOME];
	scanf("%[^\n]s", Novo.nome);
	getchar();
	//Marca
	// char Marca[TAM_MARCA];
	scanf("%[^\n]s", Novo.marca);
	getchar();
	/*-----------------------*/

	/* CAMPOS DE TAMANHO FIXO */

	//Data de Registro
	// char Data[TAM_DATA];	/* DD/MM/AAAA */
	scanf("%[^\n]s", Novo.data);
	getchar();
	//Ano de Lançamento
	// char Ano[TAM_ANO];
	scanf("%[^\n]s", Novo.ano);
	getchar();
	//Preço-Base
	// char Preço[TAM_PRECO];
	scanf("%[^\n]s", Novo.preco);
	getchar();
	//Desconto
	// char Desconto[TAM_DESCONTO];
	scanf("%[^\n]s", Novo.desconto);
	getchar();
	//Categorias
	// char Categoria[TAM_CATEGORIA];
	scanf("%[^\n]s", Novo.categoria);
	getchar();
	/*-----------------------*/

	gerarChave(&Novo);

	//Verifica se o PRODUTO existe
	// if(Busca != NULL) {
			// printf(ERRO_PK_REPETIDA, Novo->pk);
			// return;
 	// }

	// else{
		
		nregistros++;

		//Registro Auxiliar
		char rAuxiliar[193]; //TAM_REGISTRO
		rAuxiliar[192] = '\0';

		sprintf(rAuxiliar, "%s@%s@%s@%s@%s@%s@%s@%s@", Novo.pk, Novo.nome, Novo.marca, Novo.data, Novo.ano, Novo.preco, Novo.desconto, Novo.categoria);

		//Precisamos obter o TAMANHO do REGISTRO AUXILIAR (rAuxiliar) para sabermos quantos "bytes" faltam para preencher totalmento o REGISTRO.
		int Tamanho = strlen(rAuxiliar);

		// printf("\nTamanho = %d\n", Tamanho);

		int i;
		//Preenchendo o REGISTRO por completo (192bytes)
		for(i = Tamanho; i < 192; i++)
			rAuxiliar[i] = '#';

		// printf("\nTamanho - Final = %d", strlen(rAuxiliar));
		// printf("\n Registro: %s \n", rAuxiliar);

		strcat(ARQUIVO, rAuxiliar);

		//int tRegistro = strlen(rAuxiliar);
		//printf("%d\n", tRegistro);
		
		//printf("%s\n", rAuxiliar);
		
		//printf("%s\n", ARQUIVO);

		/* ATUALIZAR ÍNDICE PRIMÁRIO*/
}

