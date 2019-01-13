/**
 * ============================================================================
 * Institution : FEUP - Faculdade de Engenharia de Universidade do Porto
 * Class       : Realidade Virtual e Aumentada
 * Name        : objectload.h
 * Author      : Carlos Frias & Nuno Marques
 * Version     : 0.01
 * Description : Trabalho 1 - Realidade Aumentada com ARToolkit
 * ============================================================================
 */

#ifndef __objectload_h__
#define __objectload_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AR/ar.h>
#include <AR/arvrml.h>

#define   OBJECT_MAX       30

#ifdef __cplusplus
extern "C" {
#endif	

typedef struct {
    char       name[256];
    int        id;
    int        visible;
    double     marker_coord[4][2];
    double     trans[3][4];
	int        vrml_id;
	int        vrml_id_orig;
    double     marker_width;
    double     marker_center[2];
} ObjectData_T;

ObjectData_T *read_VRMLdata( char *name, int *objectnum, int *patternum);
static char *get_buff(char *buf, int n, FILE *fp);

#ifdef __cplusplus
}
#endif	

#endif // __objectload_h__
