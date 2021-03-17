#ifndef __SOKOBAN_FILE__
#define __SOKOBAN_FILE__


// Funções para ler níveis(tabuleiros) de Sokoban dos ficheiros de texto.
// Os tabuleiros são representados por chars com o seguinte significado:
//    # Parede
//    @ Jogador
//    + Jogador numa posição final
//    $ Caixa
//    * Caixa numa posição final
//    . Posição final
//      Espaço em branco representam o chão (posição sem nada)
//
// Para mais informação consultar:
//    http://sokobano.de/wiki/index.php?title=Level_format 
//
// Neste caso o ficheiro de tabuleiros (níveis) tem a seguinte estrutura:
//    -> um número inteiro o inicio que indica o número de tabuleiros no ficheiro
//    -> cada linha do tabuleiro está numa linha do ficheiro
//    -> cada linha do tabuleiro começa com um ' ' ou '#' (qualquer outro coisa é ignorada)
//    -> cada tabuleiro está separado do seguinte com pelo menos 1 linha em branco (vazia) (\n)
//    -> o último tabuleiro tem que ser seguide de um '\n' (portanto ter 2 linha em branco no fim)
//
// O objetivo deste jogo é o jogador empurrar todas as caixas para as posições finais, 
// logo tem que haver tantas posições finais como caixas!




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

// Função para ler níveis de um ficheiro
NiveisTab readNiveisSokoban(char* file);
// ToDo: função para apagar NiveisTab (limpar a memória)

// Funções para aceder aos níveis
int numNiveis(NiveisTab niveis);
int numLinhas(NiveisTab niveis, int iNivel);
int numColunas(NiveisTab niveis, int iNivel);
char getCelula(NiveisTab niveis, int iNivel, int lin, int col);
char getCelulaXY(NiveisTab niveis, int iNivel, int xPos, int yPos);

// função para escrever 
void printNivelSokoban(NiveisTab niveis, int iNivel);

#endif // !__SOKOBAN_FILE__


