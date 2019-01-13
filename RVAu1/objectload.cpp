/**
 * ============================================================================
 * Institution : FEUP - Faculdade de Engenharia de Universidade do Porto
 * Class       : Realidade Virtual e Aumentada
 * Name        : objectload.cpp
 * Author      : Carlos Frias & Nuno Marques
 * Version     : 0.01
 * Description : Trabalho 1 - Realidade Aumentada com ARToolkit
 * ============================================================================
 *
 * ARToolKit object parsing function
 * reads in object data from object file in Data/object_data
 *
 */

#include "objectload.h"
#pragma warning (disable : 4996)

ObjectData_T *read_VRMLdata( char *name, int *objectnum, int *patternum)
{

    FILE          *fp;
    ObjectData_T  *object;
    char           buf[256], buf1[256];
    int            i, j, temp_id;

	if( (fp=fopen( name, "r")) == NULL ) return(0);

	// recebe o numero de padroes a carregar
    get_buff(buf, 256, fp);
    //if( sscanf(buf, "%d", objectnum) != 1 ) {fclose(fp); return(0);}
    if( sscanf_s(buf, "%d", patternum) != 1 ) {fclose(fp); return(0);}

	// recebe o numero de objectos a carregar
	get_buff(buf, 256, fp);
    //if( sscanf(buf, "%d", objectnum) != 1 ) {fclose(fp); return(0);}
    if( sscanf_s(buf, "%d", objectnum) != 1 ) {fclose(fp); return(0);}

	// reserva o espaco para todos os objectos
    object = (ObjectData_T *)malloc( sizeof(ObjectData_T) * *objectnum );
    if( object == NULL ) return(0);

	// carraga todos os padroes
	for( j = 0; j < *patternum; j++ ) {
		temp_id=-1;
        get_buff(buf, 256, fp);
        //if( sscanf(buf, "%s", buf1) != 1 ) {
        if( sscanf_s(buf, "%s", buf1, sizeof(buf1)) != 1 ) {
          fclose(fp); free(object); return(0);}

		if( (temp_id=arLoadPatt(buf1)) < 0 )
            {fclose(fp); free(object); return(0);}

		printf("Patter id - %d on %s\n", temp_id, buf1);
	}
	
	// carrega todos os objectos
	for( i = 0; i < *objectnum; i++ ) {

		// carrega o tipo e nome dos objectos
        get_buff(buf, 256, fp);
		if (sscanf(buf, "%s %s", buf1, object[i].name) != 2) {
            fclose(fp); free(object); return(0);
        }
		printf("Object %d: %20s\n", i + 1, &(object[i].name[0]));
		
		// determina o tipo de objecto e carrega os modelos vrml
		if (strcmp(buf1, "VRML") == 0) {
            object[i].vrml_id = arVrmlLoadFile(object[i].name);
			printf("VRML id - %d \n", object[i].vrml_id);
            if (object[i].vrml_id < 0) {
                fclose(fp); free(object); return(0);
            }
        } else if (strcmp(buf1, "GL") == 0){
 			printf("GL object \n");
 			object[i].vrml_id = -2;
        } else {
			object[i].vrml_id = -1;
		}
		object[i].vrml_id_orig = object[i].vrml_id;
		object[i].visible = 0;

		// carrega o numero do padrao associado ao objecto
        get_buff(buf, 256, fp);
		if ( sscanf(buf, "%d", &object[i].id) != 1 ) {fclose(fp); free(object); return(0);}
		printf("Padrao id - %d \n", object[i].id);

		// altura relativa ao padrao
        get_buff(buf, 256, fp);
        if ( sscanf(buf, "%lf", &object[i].marker_width) != 1 ) {
 			fclose(fp); free(object); return(0);
		}

		// posicao relativa ao centro do padrao
        get_buff(buf, 256, fp);
        if (sscanf(buf, "%lf %lf", &object[i].marker_center[0], &object[i].marker_center[1]) != 2) {
            fclose(fp); free(object); return(0);
        }
	}

    fclose(fp);
    return( object );
}

static char *get_buff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(ret); // Skip blank lines and comments.
    }
}
