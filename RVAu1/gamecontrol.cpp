/**
 * ============================================================================
 * Institution : FEUP - Faculdade de Engenharia de Universidade do Porto
 * Class       : Realidade Virtual e Aumentada
 * Name        : gamecontrol.cpp
 * Author      : Carlos Frias & Nuno Marques
 * Version     : 0.01
 * Description : Trabalho 1 - Realidade Aumentada com ARToolkit
 * ============================================================================
 */

#include "gamecontrol.h"




//!Função que retorna a rotação em torno do eixo do Z do objeto
double calcularot(double matrizAnterior[3][4], double matrizAtual[3][4]){
	double vecXAnterior[3] = {matrizAnterior[0][0], matrizAnterior[0][1], matrizAnterior[0][2]};
	double vecXAtual[3] = {matrizAtual[0][0], matrizAtual[0][1], matrizAtual[0][2]};
	double vecX[3] = {1.0, 0.0, 0.0};
	double angulo1, angulo2;

	//Produto escalar do vetor atual em relação ao vetor (1,0,0);
	double esclr1 = escalar(vecXAtual, vecX);
	
	//Produto escalar do vetor atual em relação ao vetor (1,0,0);
	double esclr2 = escalar(vecXAnterior, vecX);
	
	//Calcula o ângulo entre os vetorAtual e (1,0,0)
	angulo1 = 180 * acosf((float) (esclr1/(modulo(vecXAtual) * modulo(vecX))))/PI;
	
	//Calcula o ângulo entre os vetorAnterior e (1,0,0)
	angulo2 = 180 * acosf((float) (esclr2/(modulo(vecXAnterior) * modulo(vecX))))/PI;
	
	//Retorna a diferença dos ângulos;
	return angulo2 - angulo1;
}

double modulo(double vec[3]){
	return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}
double escalar(double vec1[3], double vec2[3]){
	return vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
}
bool colide(double x, double y, int tabuleiro){
	int i, j, numObstaculos;
	double valX;
	double valY;
	if(tabuleiro == 0 ) numObstaculos = 3;
	else if(tabuleiro == 1) numObstaculos = 13;
	for(i = 0; i < NUM_PONTOS_COLISAO; i++){
		//pecorrer esses pontos e ver se algum está nalguma parede
		valX = x + RAIO_BOLA * cos(i * 2 * PI / NUM_PONTOS_COLISAO);
		valY = y + RAIO_BOLA * sin(i * 2 * PI / NUM_PONTOS_COLISAO);
		//printf("valX:%3.4f valY: %3.4f\n", valX, valY);
		for(j = 0; j < numObstaculos; j++){
			//Se estiver retorna false
			//printf("Xmin:%3.4f Xmax: %3.4f Ymin:%3.4f Ymax: %3.4f\n", obstaculos[j].xmin, obstaculos[j].xmax, obstaculos[j].ymin, obstaculos[j].ymax);
			if( valX > obstaculos[j].xmin && valX < obstaculos[j].xmax && 
				valY > obstaculos[j].ymin && valY < obstaculos[j].ymax) return false;
		}
	}
	//Senão retorna true
	return true;
}

void carregarObstaculos(double *xizesMin, double *xizesMax, double *ypsilonsMin, double *ypsilonsMax, int tabuleiro){
	int i, numObstaculos;
	if(tabuleiro == 0 ) numObstaculos = 3;
	else if(tabuleiro == 1) numObstaculos = 13;
	//reserva o espaco para todos os obstaculos
    obstaculos = (Obstaculo_T *)malloc(sizeof(Obstaculo_T) * numObstaculos);
	for(i = 0; i < numObstaculos; i++){
		obstaculos[i].xmin = FATOR_ESCALAMENTO * xizesMin[i];
		obstaculos[i].xmax = FATOR_ESCALAMENTO * xizesMax[i];
		obstaculos[i].ymin = FATOR_ESCALAMENTO * ypsilonsMin[i];
		obstaculos[i].ymax = FATOR_ESCALAMENTO * ypsilonsMax[i];
	}
}
bool fimJogo(double x, double y){
	double distancia = sqrt((x - xBuraco * FATOR_ESCALAMENTO) * (x - xBuraco * FATOR_ESCALAMENTO) + (y - yBuraco * FATOR_ESCALAMENTO) * (y - yBuraco * FATOR_ESCALAMENTO));
	return (distancia <= raioBuraco);
}