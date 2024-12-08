#ifndef H_MAP
#define H_MAP

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"
#include <string.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <cglm/types-struct.h>

typedef struct HillAlgoParams {
    int rows, columns, numHills, hillRadiusMin, hillRadiusMax;
    float hillHeightMin, hillHeightMax;
} HillAlgoParams;

typedef float **HeightData;

typedef struct HeightMap {
    HeightData heightData; 
    vec3s **vertices;
    int numVertices;
    vec2s **texCoords;
    vec3s **normals;
    int rows, cols;
} HeightMap;


float **gen_height_data(HillAlgoParams *params);
HeightMap *create_heightmap(HeightData height_data, HillAlgoParams *params);
void set_vertices(HeightMap *hm);
void set_tex_coords(HeightMap *hm);
void set_normals(HeightMap *hm);


#endif

