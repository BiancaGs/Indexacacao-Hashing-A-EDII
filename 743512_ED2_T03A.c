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
int  alterar(Hashtable *tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
Produto Recuperar_Registro(int RRN);

void gerarChave(Produto * Novo);

void Criar_Tabela(Hashtable *Tabela, int Tamanho);

void imprimir_tabela(Hashtable Tabela);

int Busca(char pk[], Hashtable *Tabela);

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
	//printf("Tamanho da Tabela Hash %d\n", tam);

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
					if(alterar(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 3:
					printf(INICIO_BUSCA);
					buscar(tabela);
					break;
				case 4:
					printf(INICIO_EXCLUSAO);
					if(	remover(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 5:
					printf(INICIO_LISTAGEM);
					imprimir_tabela(tabela);
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


/* Exibe o Produto */
int exibir_registro(int rrn){
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
	// for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    // 	printf("%s ", cat);
	/*COMENTAR*/
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL)
			printf(" ");
	}
	printf("\n");
	
	return 1;
}

/*Auxiliar para a função de hash*/
short f(char x){
	//printf("%c\n", x);
	return (x < 59) ? x - 48 : x - 54; 
}

/* ----------------------------------------*/

/*Função de Hash*/
short hash(const char* chave, int tam){
	
	//Função Hash
	int h = 0;
	for(int i= 1; i <= 8; i++)
		h+= i*(f(chave[i-1]));

	// printf("H %d - Tamanho %d\n", h, tam);
	// printf("H%Tamanho %d\n", h%tam);
	return (h%tam);
}

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

/*Verifica se um PRODUTO existe - Não insere PRODUTOS repetidos*/
int Busca(char pk[], Hashtable *Tabela){

	int Posicao = hash(pk, Tabela->tam);

	int flag = 0;

	//Posição Auxiliar 
	int Auxiliar =0;

	if(flag == 0 && strcmp(Tabela->v[Posicao].pk, pk) == 0){
		flag = 1;
		if(Tabela->v[Posicao].estado == OCUPADO)
			return Posicao;
		else	
			return -1;
	}
	else{
		int Auxiliar = 0;

		while(flag == 0 && Auxiliar < Tabela->tam){
			
			Auxiliar++;

			Posicao++;
			
			if(Posicao == Tabela->tam)
				Posicao = 0;

			if(strcmp(Tabela->v[Posicao].pk, pk) == 0){
				flag = 1;
				if(Tabela->v[Posicao].estado == OCUPADO)
					return Posicao;
				else	
					return -1;
			}
		}
	}

	if(flag == 0)
		return -1;
		// printf(REGISTRO_N_ENCONTRADO);
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
	if(Busca(Novo.pk, tabela) > -1) {
		printf(ERRO_PK_REPETIDA, Novo.pk);
		return;
 	}

	// else{
		
		nregistros++;

		//Registro Auxiliar
		char rAuxiliar[193]; //TAM_REGISTRO
		rAuxiliar[192] = '\0';

		sprintf(rAuxiliar, "%s@%s@%s@%s@%s@%s@%s@%s@", Novo.pk, Novo.nome, Novo.marca, Novo.data, Novo.ano, Novo.preco, Novo.desconto, Novo.categoria);

		//Precisamos obter o TAMANHO do REGISTRO AUXILIAR (rAuxiliar) para sabermos quantos "bytes" faltam para preencher totalmento o REGISTRO.
		int Tamanho = strlen(rAuxiliar);

		/*COMENTAR*/
		//printf("\nTamanho = %d", Tamanho);

		int i;
		//Preenchendo o REGISTRO por completo (192bytes)
		for(i = Tamanho; i < 192; i++)
			rAuxiliar[i] = '#';

		/*COMENTAR*/
		//printf("\nTamanho - Final = %d", strlen(rAuxiliar));
		//printf("\nRegistro: %s\n\n", rAuxiliar);

		strcat(ARQUIVO, rAuxiliar);

		//int tRegistro = strlen(rAuxiliar);
		//printf("%d\n", tRegistro);
		
		//printf("%s\n", rAuxiliar);
		
		//printf("%s\n", ARQUIVO);

		if(nregistros-1 == tabela->tam){
			printf(ERRO_TABELA_CHEIA);
			return;
		}

		/* ATUALIZAR ÍNDICE PRIMÁRIO*/
		// printf("Posicao %d\n", (Novo.pk[0]*Novo.pk[1])%tabela->tam);
		int Posicao = hash(Novo.pk, tabela->tam);

		// printf("Posicao - Funcao Hash %d\n", Posicao);
		
		int flag = 0;
		
		/* Insere em Local com ESTADO REMOVIDO?*/
		if(tabela->v[Posicao].estado == 0 || tabela->v[Posicao].estado == 2){
			// printf("Posicao Livre - Chave %s\n", Novo.pk);
			tabela->v[Posicao].estado = 1;
			strcpy(tabela->v[Posicao].pk, Novo.pk);
			tabela->v[Posicao].rrn = nregistros-1;

			flag = 1;

			printf(REGISTRO_INSERIDO, Novo.pk, 0);
			return;
		}

		//else{ //if(tabela->v[Posicao].estado == 1){ //|| tabela->v[Posicao].estado == 2){

			int Colisoes = 0;

			//Posição Auxiliar
			int Auxiliar = 0;

			while(flag == 0 && Auxiliar < tabela->tam ){ //&& Posicao < tabela->tam){				
				

				Colisoes++;

				Auxiliar++;

				Posicao++;
				if(Posicao == tabela->tam)
					Posicao = 0;
					
				if(tabela->v[Posicao].estado == LIVRE || tabela->v[Posicao].estado == REMOVIDO){		
				
					flag = 1;

					tabela->v[Posicao].estado = 1;
					strcpy(tabela->v[Posicao].pk, Novo.pk);
					tabela->v[Posicao].rrn = nregistros-1;
					// printf("RRN %d\n", tabela->v[Posicao].rrn);

					printf(REGISTRO_INSERIDO, Novo.pk, Colisoes);
					return;
				}
				
			}
		//}
		if(flag == 0)
			printf(ERRO_TABELA_CHEIA);
}

int  alterar(Hashtable *tabela){

	char PK[TAM_PRIMARY_KEY];
	memset(PK, '\0', TAM_PRIMARY_KEY);

	scanf("%[^\n]s", PK);
	getchar();

	if(Busca(PK, tabela) == -1)
		printf(REGISTRO_N_ENCONTRADO);

	else{
		int RRN = tabela->v[Busca(PK, tabela)].rrn;
		//printf("%d\n", RRN);
		
		//O desconto inserido precisa ser de 3 bytes com valor entre 000 e 100.
		char Desconto[3];

		int flag = 0;				
		//getchar();
		scanf("%[^\n]s", Desconto);
		getchar();

		if(strcmp(Desconto, "000") >= 0 && strcmp(Desconto, "100") <= 0)
			flag = 1;

		while(flag == 0){	
			printf(CAMPO_INVALIDO);
			//getchar();	
			scanf("%[^\n]s", Desconto);
			getchar();
			
			if(strcmp(Desconto, "000") >= 0 && strcmp(Desconto, "100") <= 0)
				flag = 1;
		}

		char *Arquivo = ARQUIVO + RRN * 192;
		// printf("%s\n", Arquivo);

		int Contador = 0;
		while(Contador < 6){
			if((*Arquivo) == '@')
				Contador++;
			Arquivo++;
		}
		//printf("Contador %d\n", Contador);
		// printf("%s\n", Arquivo);

		for(int i = 0; i < 3; i++){
			*Arquivo = Desconto[i];
			Arquivo++;
		}
		return 1;
	}
}

void buscar(Hashtable tabela){
	
	char pk[TAM_PRIMARY_KEY];
	memset(pk, '\0', TAM_PRIMARY_KEY);

	scanf("%[^\n]s", pk);
	getchar();

	int Posicao = hash(pk, tabela.tam);

	int flag = 0;
	if(flag == 0 && strcmp(tabela.v[Posicao].pk, pk) == 0){
		exibir_registro(tabela.v[Posicao].rrn);
		flag = 1;
		//printf("%d\n", tabela.v[Posicao].rrn);
		return;
	}
	else{
		int Auxiliar = 0;
		while(flag == 0 && Auxiliar < tabela.tam){
			
			Auxiliar++;

			Posicao++;
			
			if(Posicao == tabela.tam)
				Posicao = 0;

			if(strcmp(tabela.v[Posicao].pk, pk) == 0){
				if(tabela.v[Posicao].estado == 1){
					exibir_registro(tabela.v[Posicao].rrn);
					flag = 1;
					//printf("%d\n", tabela.v[Posicao].rrn);
					return;
				}
			}
		}
	}

	if(flag == 0)
		printf(REGISTRO_N_ENCONTRADO);
}

int  remover(Hashtable* tabela){

	char pk[TAM_PRIMARY_KEY];
	memset(pk, '\0', TAM_PRIMARY_KEY);

	scanf("%[^\n]s", pk);
	getchar();

	if(Busca(pk, tabela) == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	
	else{
		tabela->v[Busca(pk, tabela)].estado = 2;
		return 1;
	}
}

void imprimir_tabela(Hashtable Tabela){

	for(int i=0; i<Tabela.tam; i++){
		if(Tabela.v[i].estado == 0){
			printf(POS_LIVRE, i);
		}
		else if(Tabela.v[i].estado == 1){
			printf(POS_OCUPADA, i, Tabela.v[i].pk);
		}
		else if(Tabela.v[i].estado == 2){
			printf(POS_REMOVIDA, i);
		}
	}
}
