/**
 * ============================================================================
 * Institution : FEUP - Faculdade de Engenharia de Universidade do Porto
 * Class       : Realidade Virtual e Aumentada
 * Name        : RVAu.h
 * Author      : Carlos Frias & Nuno Marques
 * Version     : 0.01
 * Description : Trabalho 1 - Realidade Aumentada com ARToolkit
 * ============================================================================
 */

// ============================================================================
//	Includes
// ============================================================================

#ifdef _WIN32
#  include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>			// arParamDisp()
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/arvrml.h>
#include <AR/param.h>
#include <AR/gsub.h>

#include "objectload.h"
#include "gamecontrol.h"

// ============================================================================
//	Constants
// ============================================================================

#define VIEW_SCALEFACTOR		0.025		// 1.0 ARToolKit unit becomes 0.025 of my OpenGL units.
#define VIEW_SCALEFACTOR_1		1.0			// 1.0 ARToolKit unit becomes 1.0 of my OpenGL units.
#define VIEW_SCALEFACTOR_4		4.0			// 1.0 ARToolKit unit becomes 4.0 of my OpenGL units.
#define VIEW_DISTANCE_MIN		4.0			// Objects closer to the camera than this will not be displayed.
#define VIEW_DISTANCE_MAX		4000.0		// Objects further away from the camera than this will not be displayed.

#define WINDOW_TITLE_PREFIX "Trabalho 1 - Realidade Aumentada com ARToolkit"

// ============================================================================
//	Global variables
// ============================================================================

// Camera
#ifdef _WIN32
	char			*vconf = "Data\\WDM_camera_flipV.xml"; //  saida inverta do video
	//char			*vconf = "Data\\WDM_camera.xml"; // saida direta do video
#else
	char			*vconf = "";
#endif

const char *cparam_name = "Data/camera_para.dat"; //name of the camera parameter file

// Preferences.
static int prefWindowed = TRUE;
static int prefWidth = 640;					// Fullscreen mode width.
static int prefHeight = 480;				// Fullscreen mode height.
static int prefDepth = 32;					// Fullscreen mode bit depth.
static int prefRefresh = 0;					// Fullscreen mode refresh rate. Set to 0 to use default rate.


// Image acquisition.
static ARUint8		*gARTImage = NULL;

// Marker detection.
static int			gARTThreshhold = 100;
static long			gCallCountMarkerDetect = 0;
static int			gPatterDataCount;

// Transformation matrix retrieval.
static int			gPatt_found = FALSE;	// At least one marker.

// Drawing.
static ARParam		gARTCparam;
static ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;

// Object Data.
static ObjectData_T	*gObjectData;
static int			gObjectDataCount;
char				objectDataFilename[] = "Data/object_data_rvau1"; //the object data file



double			rotacaoX = 0.0;
double			rotacaoY = 0.0;
double			matrizAnterior1[3][4] = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
double			matrizAtual1[3][4] = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
double			matrizAnterior2[3][4] = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
double			matrizAtual2[3][4] = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
int				flag1 = 0;
int				flag2 = 0;
double			rot_X = 0.0;
double			rot_Y = 0.0;
double			trans_Y = -32.0 * 20;
double			trans_X = -27.0 * 20;
int				BOLA = 2;
int				COMANDO1 = 4;
int				COMANDO2 = 5; 

const double	FATOR_TRANS = 5.0;
const double	MAX_ROT	= 8.2;
const double	MIN_ROT = 0.05;
const double	MAX_Y = 600.0;
const double	MAX_X = 800.0;

int				NumObstaculos = 3;
int				TAB_SELECIONADO = 0;

/////
///Valores para os obstáculos do primeiro tabuleiro
double xizesMin[3] = {-30.0, -15.0, -30.0};
double xizesMax[3] = {-30.0 + 45.0,-15.0 + 45.0, -30.0 + 45.0};
double ypsilonsMin[3] = {-21.0, -1.0, 19.0};
double ypsilonsMax[3] = {-21.0 + 2.0,-1.0 + 2.0, 19.0 + 2.0};

double xizesMin1[13] =    {-16.0,  15.0,  -1.0, -30.0,  14.0,  14.0, -30.0, -16.0, 14.0, -16.0,  -1.0,  -1.0, -30.0};
double xizesMax1[13] =    {-14.0,  30.0,   1.0,   1.0,  16.0,  30.0,   0.0, -14.0, 16.0,  16.0,   1.0,  30.0, -15.0};
double ypsilonsMin1[13] = {-40.0, -31.0, -30.0, -21.0, -10.0,  -1.0,  -1.0,  10.0, 10.0,  19.0,  21.0,  29.0,  29.0};
double ypsilonsMax1[13] = {-30.0, -29.0, -21.0, -19.0,  -1.0,   1.0,   1.0,  19.0, 19.0,  21.0,  29.0,  31.0,  31.0};
//////

// ============================================================================
//	Functions
// ============================================================================

int main(int argc, char** argv); // função inicial da aplicação
static int setupCamera(const char *cparam_name, char *vconf, ARParam *cparam); // Configurar parametros da camera
static void debugReportMode(void); // Report state of ARToolKit global variables arFittingMode, arImageProcMode, arglDrawMode, arTemplateMatchingMode, arMatchingPCAMode.
static int setupMarkersObjects(char *objectDataFilename); // Load in the object data, trained markers and associated bitmap files.
static void Display(void); // This function is called when the window needs redrawing.
static void Reshape(int w, int h); // This function is called when the GLUT window is resized.
static void Visibility(int visible); // This function is called on events when the visibility of the GLUT window changes
static void Idle(void); // sets the global idle callback to be func so a GLUT program can perform background  processing  tasks  or
						// continuous  animation  when  window  system events are not being received.
static void Keyboard(unsigned char key, int x, int y); // mapea as teclas disponiveis para iteragir com a aplicacao
static void Quit(void); // Limpa as configuracoes do AR, termina a captacao do video e fecha-o
void atualizarMatrizes(ObjectData_T	*gObjectDataAt, int i, int j, int numMatriz); //Funções que atualizam as matrizes de transformação do tabuleiro j e da bola i
void restartGame();
void animaObjeto(int indice);