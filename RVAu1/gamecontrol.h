/**
 * ============================================================================
 * Institution : FEUP - Faculdade de Engenharia de Universidade do Porto
 * Class       : Realidade Virtual e Aumentada
 * Name        : gamecontrol.h
 * Author      : Carlos Frias & Nuno Marques
 * Version     : 0.01
 * Description : Trabalho 1 - Realidade Aumentada com ARToolkit
 * ============================================================================
 */

#ifndef __gamecontrol_h__
#define __gamecontrol_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "objectload.h"

//
#define PI 3.14159265359

typedef struct {
	double xmin;
	double xmax;
	double ymin;
	double ymax;
} Obstaculo_T;

const int		NUM_PONTOS_COLISAO = 100;
const double	RAIO_BOLA = 60.0;
static Obstaculo_T *obstaculos;
const double	FATOR_ESCALAMENTO = 20.0;

const double	xBuraco = 25.0;
const double	yBuraco = 35.0;
const double	raioBuraco = 70.0;

double calcularot(double matrizAnterior[3][4], double matrizAtual[3][4]); //!Função que retorna a rotação em torno do eixo do Z do objeto
double modulo(double vec[3]); // calcula o modulo do vector
double escalar(double vec1[3], double vec2[3]); // calcula o escalar de um vector
bool colide(double x, double y, int tabuleiro); //Função que averigua se a bola está ou não a colidir com um obstáculo
void carregarObstaculos(double *xizesMin, double *xizesMax, double *ypsilonsMin, double *ypsilonsMax, int tabuleiro);
bool fimJogo(double x, double y); //Função que averigua se a bola cai no buraco


#endif // __gamecontrol_h__
