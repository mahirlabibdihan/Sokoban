#ifndef __SOKOBAN_FILE__
#define __SOKOBAN_FILE__


// Fun��es para ler n�veis(tabuleiros) de Sokoban dos ficheiros de texto.
// Os tabuleiros s�o representados por chars com o seguinte significado:
//    # Parede
//    @ Jogador
//    + Jogador numa posi��o final
//    $ Caixa
//    * Caixa numa posi��o final
//    . Posi��o final
//      Espa�o em branco representam o ch�o (posi��o sem nada)
//
// Para mais informa��o consultar:
//    http://sokobano.de/wiki/index.php?title=Level_format 
//
// Neste caso o ficheiro de tabuleiros (n�veis) tem a seguinte estrutura:
//    -> um n�mero inteiro o inicio que indica o n�mero de tabuleiros no ficheiro
//    -> cada linha do tabuleiro est� numa linha do ficheiro
//    -> cada linha do tabuleiro come�a com um ' ' ou '#' (qualquer outro coisa � ignorada)
//    -> cada tabuleiro est� separado do seguinte com pelo menos 1 linha em branco (vazia) (\n)
//    -> o �ltimo tabuleiro tem que ser seguide de um '\n' (portanto ter 2 linha em branco no fim)
//
// O objetivo deste jogo � o jogador empurrar todas as caixas para as posi��es finais, 
// logo tem que haver tantas posi��es finais como caixas!




typedef struct tabuleiroChars* TabuleiroChars;
struct tabuleiroChars {
	char** celula;
	int linhas, colunas;
};


typedef struct niveisTab* NiveisTab;
struct niveisTab {
	TabuleiroChars* tabs;
	int numTabs;
};

// Fun��o para ler n�veis de um ficheiro
NiveisTab readNiveisSokoban(char* file);
// ToDo: fun��o para apagar NiveisTab (limpar a mem�ria)

// Fun��es para aceder aos n�veis
int numNiveis(NiveisTab niveis);
int numLinhas(NiveisTab niveis, int iNivel);
int numColunas(NiveisTab niveis, int iNivel);
char getCelula(NiveisTab niveis, int iNivel, int lin, int col);
char getCelulaXY(NiveisTab niveis, int iNivel, int xPos, int yPos);

// fun��o para escrever 
void printNivelSokoban(NiveisTab niveis, int iNivel);

#endif // !__SOKOBAN_FILE__


