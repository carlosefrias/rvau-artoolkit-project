/**
 * ============================================================================
 * Institution : FEUP - Faculdade de Engenharia de Universidade do Porto
 * Class       : Realidade Virtual e Aumentada
 * Name        : RVAu1.cpp
 * Author      : Carlos Frias & Nuno Marques
 * Version     : 0.01
 * Description : Trabalho 1 - Realidade Aumentada com ARToolkit
 * ============================================================================
 */

#include "RVAu1.h"
#pragma warning (disable : 4996)

int main(int argc, char** argv)
{
	int i;
	char glutGamemode[32];

	//Carregando os obstáculos
	if(TAB_SELECIONADO == 0) carregarObstaculos(xizesMin, xizesMax, ypsilonsMin, ypsilonsMax, TAB_SELECIONADO);
	else if(TAB_SELECIONADO == 1) carregarObstaculos(xizesMin1, xizesMax1, ypsilonsMin1, ypsilonsMax1, TAB_SELECIONADO);

	//
	// ----------------------------------------------------------------------------
	// Library inits.
	glutInit(&argc, argv);

	// ----------------------------------------------------------------------------
	// Hardware setup.
	if (!setupCamera(cparam_name, vconf, &gARTCparam)) {
		fprintf(stderr, "main(): Unable to set up AR camera.\n");
		exit(-1);
	}
	
#ifdef _WIN32
	CoInitialize(NULL);
#endif

	// ----------------------------------------------------------------------------
	// Library setup.
	// Set up GL context(s) for OpenGL to draw into.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	if (!prefWindowed) {
		if (prefRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", prefWidth, prefHeight, prefDepth, prefRefresh);
		else sprintf(glutGamemode, "%ix%i:%i", prefWidth, prefHeight, prefDepth);
		glutGameModeString(glutGamemode);
		glutEnterGameMode();
	} else {
		glutInitWindowSize(gARTCparam.xsize, gARTCparam.ysize);
		glutCreateWindow(WINDOW_TITLE_PREFIX);
	}

	// Setup argl library for current context.
	if ((gArglSettings = arglSetupForCurrentContext()) == NULL) {
		fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
		exit(-1);
	}
	debugReportMode();
	arUtilTimerReset();

	if (!setupMarkersObjects(objectDataFilename)) {
		fprintf(stderr, "main(): Unable to set up AR objects and markers.\n");
		Quit();
	}
	
	// Test render all the VRML objects.
    fprintf(stdout, "Pre-rendering the VRML objects...");
	fflush(stdout);
    glEnable(GL_TEXTURE_2D);
    for (i = 0; i < gObjectDataCount; i++) {
		if(	gObjectData[i].vrml_id >= 0)
			arVrmlDraw(gObjectData[i].vrml_id);
    }
    glDisable(GL_TEXTURE_2D);
	fprintf(stdout, " done\n");
	
	// Register GLUT event-handling callbacks.
	// NB: Idle() is registered by Visibility.
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutVisibilityFunc(Visibility);
	glutKeyboardFunc(Keyboard);
	
	glutMainLoop();

	return (0);
}

// Configurar parametros da camera
static int setupCamera(const char *cparam_name, char *vconf, ARParam *cparam)
{	
    ARParam			wparam;
	int				xsize, ysize;

    // Open the video path.
    if (arVideoOpen(vconf) < 0) {
    	fprintf(stderr, "setupCamera(): Unable to open connection to camera.\n");
    	return (FALSE);
	}
	
    // Find the size of the window.
    if (arVideoInqSize(&xsize, &ysize) < 0)
		return (FALSE);
    fprintf(stdout, "Camera image size (x,y) = (%d,%d)\n", xsize, ysize);
	
	// Load the camera parameters, resize for the window and init.
    if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		fprintf(stderr, "setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
        return (FALSE);
    }

	// Arterar tamanho da janela
    arParamChangeSize(&wparam, xsize, ysize, cparam);
    fprintf(stdout, "*** Camera Parameter ***\n");
    arParamDisp(cparam);
	
    arInitCparam(cparam);

	// Inicia a captuara da camera
	if (arVideoCapStart() != 0) {
    	fprintf(stderr, "setupCamera(): Unable to begin camera data capture.\n");
		return (FALSE);		
	}
	
	return (TRUE);
}

// Report state of ARToolKit global variables arFittingMode,
// arImageProcMode, arglDrawMode, arTemplateMatchingMode, arMatchingPCAMode.
static void debugReportMode(void)
{
	if(arFittingMode == AR_FITTING_TO_INPUT ) {
		fprintf(stderr, "FittingMode (Z): INPUT IMAGE\n");
	} else {
		fprintf(stderr, "FittingMode (Z): COMPENSATED IMAGE\n");
	}
	
	if( arImageProcMode == AR_IMAGE_PROC_IN_FULL ) {
		fprintf(stderr, "ProcMode (X)   : FULL IMAGE\n");
	} else {
		fprintf(stderr, "ProcMode (X)   : HALF IMAGE\n");
	}
	
	if (arglDrawModeGet(gArglSettings) == AR_DRAW_BY_GL_DRAW_PIXELS) {
		fprintf(stderr, "DrawMode (C)   : GL_DRAW_PIXELS\n");
	} else if (arglTexmapModeGet(gArglSettings) == AR_DRAW_TEXTURE_FULL_IMAGE) {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (FULL RESOLUTION)\n");
	} else {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (HALF RESOLUTION)\n");
	}
		
	if( arTemplateMatchingMode == AR_TEMPLATE_MATCHING_COLOR ) {
		fprintf(stderr, "TemplateMatchingMode (M)   : Color Template\n");
	} else {
		fprintf(stderr, "TemplateMatchingMode (M)   : BW Template\n");
	}
	
	if( arMatchingPCAMode == AR_MATCHING_WITHOUT_PCA ) {
		fprintf(stderr, "MatchingPCAMode (P)   : Without PCA\n");
	} else {
		fprintf(stderr, "MatchingPCAMode (P)   : With PCA\n");
	}
}

// Load in the object data, trained markers and associated bitmap files.
static int setupMarkersObjects(char *objectDataFilename)
{	
	if ((gObjectData = read_VRMLdata(objectDataFilename, &gObjectDataCount, &gPatterDataCount)) == NULL) {
        fprintf(stderr, "setupMarkersObjects(): read_VRMLdata returned error !!\n");
        return (FALSE);
    }

    printf("Object count = %d\n", gObjectDataCount);
	
	return (TRUE);
}

// This function is called when the window needs redrawing.
static void Display(void)
{
	int i,j;
    GLdouble p[16];
	GLdouble m[16];
	
	/////////////////////////////
	//Atualizar as matrizes anteriores
	for(i = 0; i < 3; i++){
		for(j = 0; j < 4; j++){
			matrizAnterior1[i][j] = matrizAtual1[i][j];
			matrizAnterior2[i][j] = matrizAtual2[i][j];
		}
	}

	/////////////////////////////



	// Select correct buffer for this context.
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers for new frame.
	
	arglDispImage(gARTImage, &gARTCparam, 1.0, gArglSettings);	// zoom = 1.0.
	arVideoCapNext();
	gARTImage = NULL; // Image data is no longer valid after calling arVideoCapNext().
				
	if (gPatt_found) {
		// Projection transformation.
		arglCameraFrustumRH(&gARTCparam, VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(p);
		glMatrixMode(GL_MODELVIEW);
		
		// Viewing transformation.
		glLoadIdentity();
		// Lighting and geometry that moves with the camera should go here.
		// (I.e. must be specified before viewing transformations.)
		//none
		
		// All other lighting and geometry goes here.
		// Calculate the camera position for each object and draw it.
		for (i = 0; i < gObjectDataCount; i++) {
			if(TAB_SELECIONADO == 0) {
				gObjectData[1].visible = 0;
				gObjectData[0].visible = 1;
			}
			else if(TAB_SELECIONADO == 1){
				gObjectData[0].visible = 0;
				gObjectData[1].visible = 1;
			}
			if ((gObjectData[i].visible != 0) && (gObjectData[i].vrml_id >= 0)) {
				//fprintf(stderr, "About to draw object %i\n", i);
				arglCameraViewRH(gObjectData[i].trans, m, VIEW_SCALEFACTOR_4);
				glLoadMatrixd(m);

				atualizarMatrizes(gObjectData, COMANDO2, TAB_SELECIONADO, 1);
				atualizarMatrizes(gObjectData, COMANDO1, TAB_SELECIONADO, 2);
				animaObjeto(i);
				arVrmlDraw(gObjectData[i].vrml_id);
			}			
		}
	} // gPatt_found
	
	// Any 2D overlays go here.
	//none
	
	glutSwapBuffers();
}

//
//	This function is called when the
//	GLUT window is resized.
//
static void Reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Call through to anyone else who needs to know about window sizing here.
}

//
//	This function is called on events when the visibility of the
//	GLUT window changes (including when it first becomes visible).
//
static void Visibility(int visible)
{
	if (visible == GLUT_VISIBLE) {
		glutIdleFunc(Idle);
	} else {
		glutIdleFunc(NULL);
	}
}

// sets the global idle callback to be func so a GLUT program can perform background 
// processing  tasks  or continuous  animation  when  window  system events are not being received.
static void Idle(void)
{
	static int ms_prev;
	int ms;
	float s_elapsed;
	ARUint8 *image;

	ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
    int             marker_num;						// Count of number of markers detected.
    int             i, j, k;
	
	// Find out how long since Idle() last ran.
	ms = glutGet(GLUT_ELAPSED_TIME);
	s_elapsed = (float)(ms - ms_prev) * 0.001;
	if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
	ms_prev = ms;
	
	// Update drawing.
	arVrmlTimerUpdate();
	
	// Grab a video frame.
	if ((image = arVideoGetImage()) != NULL) {
		gARTImage = image;	// Save the fetched image.
		gPatt_found = FALSE;	// Invalidate any previous detected markers.
		
		gCallCountMarkerDetect++; // Increment ARToolKit FPS counter.
		
		// Detect the markers in the video frame.
		if (arDetectMarker(gARTImage, gARTThreshhold, &marker_info, &marker_num) < 0) {
			exit(-1);
		}
				
		// Check for object visibility.
		for (i = 0; i < gObjectDataCount; i++) {
		
			// Check through the marker_info array for highest confidence
			// visible marker matching our object's pattern.
			k = -1;
			for (j = 0; j < marker_num; j++) {
				if (marker_info[j].id == gObjectData[i].id) {
					if( k == -1 ) k = j; // First marker detected.
					else if (marker_info[k].cf < marker_info[j].cf) k = j; // Higher confidence marker detected.
				}
			}
			
			if (k != -1) {
				// Get the transformation between the marker and the real camera.
				//fprintf(stderr, "Saw object %d.\n", i);
				if (gObjectData[i].visible == 0) {
					arGetTransMat(&marker_info[k],
								  gObjectData[i].marker_center,
								  gObjectData[i].marker_width,
								  gObjectData[i].trans);
				} else {
					arGetTransMatCont(&marker_info[k],
									  gObjectData[i].trans,
									  gObjectData[i].marker_center,
									  gObjectData[i].marker_width,
									  gObjectData[i].trans);
				}
				gObjectData[i].visible = 1;
				gPatt_found = TRUE;
			} else {
				gObjectData[i].visible = 0;
			}
		}

		
		// Tell GLUT to update the display.
		glutPostRedisplay();
	}
}

// mapea as teclas disponiveis para iteragir com a aplicacao
static void Keyboard(unsigned char key, int x, int y)
{
	int mode;
	switch (key) {
		case 0x1B:						// Quit.
		case 'Q':
		case 'q':
			Quit();
			break;
		case 'C':
		case 'c':
			mode = arglDrawModeGet(gArglSettings);
			if (mode == AR_DRAW_BY_GL_DRAW_PIXELS) {
				arglDrawModeSet(gArglSettings, AR_DRAW_BY_TEXTURE_MAPPING);
				arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_FULL_IMAGE);
			} else {
				mode = arglTexmapModeGet(gArglSettings);
				if (mode == AR_DRAW_TEXTURE_FULL_IMAGE)	arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_HALF_IMAGE);
				else arglDrawModeSet(gArglSettings, AR_DRAW_BY_GL_DRAW_PIXELS);
			}
			fprintf(stderr, "*** Camera - %f (frame/sec)\n", (double)gCallCountMarkerDetect/arUtilTimer());
			gCallCountMarkerDetect = 0;
			arUtilTimerReset();
			debugReportMode();
			break;
		case '?':
		case '/':
			printf("Keys:\n");
			printf(" q or [esc]    Quit demo.\n");
			printf(" c             Change arglDrawMode and arglTexmapMode.\n");
			printf(" ? or /        Show this help.\n");
			printf("\nAdditionally, the ARVideo library supplied the following help text:\n");
			arVideoDispOption();
			break;
		case 't':
		case 'T':
			if(TAB_SELECIONADO == 0){
				TAB_SELECIONADO = 1;
			}else if(TAB_SELECIONADO == 1){
				TAB_SELECIONADO = 0;
			}
			restartGame();
			break;
		case 'r':
		case 'R':
			restartGame();
			break;
		default:
			break;
	}
}

// Limpa as configuracoes do AR, termina a captacao do video e fecha-o
static void Quit(void)
{
	arglCleanup(gArglSettings);
	arVideoCapStop();
	arVideoClose();
#ifdef _WIN32
	CoUninitialize();
#endif
	exit(0);
}

void atualizarMatrizes(ObjectData_T	*gObjectDataAt, int i, int j, int numMatriz){
	int k, l;
	if(gObjectDataAt[i].visible != 0 && gObjectDataAt[j].visible != 0){
		//Atualizar a matriz atual
		for(k = 0; k < 3; k++){
			for(l = 0; l < 4; l++){
				if(numMatriz == 1)
					matrizAtual1[k][l] = gObjectDataAt[i].trans[k][l];
				else 
					matrizAtual2[k][l] = gObjectDataAt[i].trans[k][l];
			}
		}
		if(flag1 == 0) {
			rotacaoY = 0.0;
			flag1 = 1;
		}
		else if(numMatriz == 1){
				rotacaoY = calcularot(matrizAnterior1, matrizAtual1);
		}

		if(flag2 == 0) {
			rotacaoX = 0.0;
			flag2 = 1;
		}
		else if(numMatriz == 2){
			rotacaoX = calcularot(matrizAnterior2, matrizAtual2);
		}
	}
}
void restartGame(){
	//Colocar a bola na posição inicial
	trans_Y = -32.0 * 20;
	trans_X = -27.0 * 20;
	//Colocar as flag's a zero
	flag1 = 0;
	flag2 = 0;
	//colocar a rotação do tabuleiro a zero
	rot_X = 0.0;
	rot_Y = 0.0;
	rotacaoX = 0.0;
	rotacaoY = 0.0;
	int i, j;
	//colocar as matrizes a zero
	for(i = 0; i < 3; i++){
		for(j = 0; j < 4; j++){
			matrizAnterior1[i][j] = 0.0;
			matrizAtual1[i][j] = 0.0;
			matrizAnterior2[i][j] = 0.0;
			matrizAtual2[i][j] = 0.0;
		}
	}
	//Carrega os obstaculos corretos
	if(TAB_SELECIONADO == 0) carregarObstaculos(xizesMin, xizesMax, ypsilonsMin, ypsilonsMax, TAB_SELECIONADO);
	else if(TAB_SELECIONADO == 1) carregarObstaculos(xizesMin1, xizesMax1, ypsilonsMin1, ypsilonsMax1, TAB_SELECIONADO);
}
void animaObjeto(int indice){
	//No caso do tabuleiro ou da bola
	if(indice == TAB_SELECIONADO || indice == BOLA){		
					
		if(abs(rotacaoY) > MIN_ROT) rot_Y += rotacaoY;
		if(abs(rotacaoX) > MIN_ROT) rot_X += rotacaoX;

		if(rot_Y > MAX_ROT) rot_Y = MAX_ROT;
		if(rot_Y < -MAX_ROT) rot_Y = -MAX_ROT;
		if(rot_X > MAX_ROT) rot_X = MAX_ROT;
		if(rot_X < -MAX_ROT) rot_X = -MAX_ROT;
		glRotated(rot_Y, 1, 0, 0);
		glRotated(rot_X, 0, 1, 0);
					
		//No caso da bola
		if(indice == BOLA){
			double auxY = trans_Y;
			double auxX = trans_X;

			trans_Y += rot_Y * FATOR_TRANS;
			trans_X += rot_X * FATOR_TRANS;

			if(trans_Y + RAIO_BOLA >= MAX_Y) {
				trans_Y = MAX_Y - RAIO_BOLA;
			}
			if(trans_Y - RAIO_BOLA <= -MAX_Y) {
				trans_Y = -MAX_Y + RAIO_BOLA;
			}
			if(trans_X + RAIO_BOLA >= MAX_X){
				trans_X = MAX_X - RAIO_BOLA;
			}
			if(trans_X - RAIO_BOLA <= -MAX_X){
				trans_X = - MAX_X + RAIO_BOLA;
			}
			if(!colide(trans_Y, trans_X, TAB_SELECIONADO)){
				if(colide(auxY, trans_X, TAB_SELECIONADO)){
					trans_Y = auxY;
				}
				else if(colide(trans_Y, auxX, TAB_SELECIONADO)){
					trans_X = auxX;
				}
				else{
					trans_Y = auxY - rot_Y * FATOR_TRANS / 100;
					trans_X = auxX - rot_X * FATOR_TRANS / 100;
				}
			}
			//TODO rotação da bola
			glTranslated(trans_X, -trans_Y, 0.0);
			if(fimJogo(-trans_Y, trans_X)){
				MessageBox(NULL, "Parabéns ganhou o jogo!!", "FIM", MB_OK | MB_ICONINFORMATION);
				restartGame();
			}
		}
	}
}