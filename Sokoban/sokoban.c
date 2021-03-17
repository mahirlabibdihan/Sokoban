#include "sokoban.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(X,Y) X>Y?X:Y
#define SIZEMAX 2048

char* strClones[SIZEMAX];
int numClones = 0;
char buffer[SIZEMAX];

char* cloneString(char* str)
{
	char* strClone;
	int len = strlen(str);
	int i;

	//ignore '\n' (last char)
	str[--len] = '\0';

	strClone = malloc(sizeof(char) * (len + 1));
	for (i = 0; i <= len; i++)
		strClone[i] = str[i];

	return strClone;
}

int maiorClones()
{
	int i;
	int m = 0;
	int len;

	for (i = 0; i < numClones; i++) {
		len = strlen(strClones[i]);
		m = MAX(m, len);
	}

	return m;
}

void copyFromClone(char* dest, char* clone)
{
	int i=0;

	// copia os char do clone para o dest
	while (clone[i] != '\0') {
		dest[i] = clone[i];
		i++;
	}

	// as restantes posições do dest ficas com espaços em branco
	while (dest[i] != '\0') dest[i++] = ' ';

}

void cleanClones()
{
	int i;
	for (i = 0; i < numClones; i++)
		free(strClones[i]);
	numClones = 0;
}

TabuleiroChars criaNivelFromClone()
{
	TabuleiroChars tab;
	int i;

	if (numClones == 0) return NULL;  // em principio isto nunca deve acontecer...

	tab = malloc(sizeof(struct tabuleiroChars));
	tab->linhas = numClones;
	tab->colunas = maiorClones();
	tab->celula = malloc(sizeof(char*) * tab->linhas);
	
	for (i = 0; i < tab->linhas; i++) {
		tab->celula[i] = malloc(sizeof(char) * (tab->colunas + 1));
		tab->celula[i][tab->colunas] = '\0';
		copyFromClone(tab->celula[i], strClones[tab->linhas - 1 - i]);
	}

	// need to clean the clones!!!!
	cleanClones();

	return tab;
}

TabuleiroChars readTabFromFile(FILE* fic)
{
	numClones = 0;
	while (!feof(fic)) {
		fgets(buffer, SIZEMAX - 1, fic);
		if (buffer[0] == ' ' || buffer[0] == '#')  // linha do tabuleiro, copia para os clones
			strClones[numClones++] = cloneString(buffer);
		if (buffer[0] == '\n' && numClones > 0)  // chegou ao fim do tabuleiro, cria nível a  partir dos clones
			return criaNivelFromClone();
	}
	return criaNivelFromClone();
}


NiveisTab readNiveisSokoban(char* file)
{
	FILE* fic;
	int i;
	NiveisTab niveis;

	fopen_s(&fic, file, "r");
	if (fic == NULL) {
		printf("ERRO na abertura do ficheiro %s\n\n");
		exit(0);
	}

	niveis = malloc(sizeof(struct niveisTab));
	fscanf_s(fic, "%i", &(niveis->numTabs));
	niveis->tabs = malloc(sizeof(struct tabuleiroChars) * niveis->numTabs);

	for (i = 0; i < niveis->numTabs; i++) {
		niveis->tabs[i] = readTabFromFile(fic);
	}
	return niveis;
}


void printNivelSokoban(NiveisTab niveis, int iNivel)
{
	TabuleiroChars nivel = niveis->tabs[iNivel];
	int i, j;
	
	for (i = nivel->linhas - 1; i >= 0 ; i--) {
		for (j = 0; j < nivel->colunas; j++) {
			printf("%c", nivel->celula[i][j]);
		}
		printf("\n");
	}
}


int numNiveis(NiveisTab niveis)
{
	return niveis->numTabs;
}

int numLinhas(NiveisTab niveis, int iNivel)
{
	return niveis->tabs[iNivel]->linhas;
}

int numColunas(NiveisTab niveis, int iNivel)
{
	return niveis->tabs[iNivel]->colunas;
}

char getCelula(NiveisTab niveis, int iNivel, int lin, int col)
{
	return niveis->tabs[iNivel]->celula[lin][col];
}

char getCelulaXY(NiveisTab niveis, int iNivel, int xPos, int yPos)
{
	return niveis->tabs[iNivel]->celula[yPos][xPos];
}




