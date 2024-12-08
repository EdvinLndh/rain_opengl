#include "heightmap.h"
#include "util.h"

// Utility function to generate random float in range [min, max]
float random_float(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

// Utility function to generate random int in range [min, max]
int random_int(int min, int max) { return min + rand() % (max - min + 1); }

HeightData gen_height_data(HillAlgoParams *params) {
  // Allocate memory for heightData (2D array)
  HeightData heightData = malloc(params->rows * sizeof(float *));
  for (int i = 0; i < params->rows; i++) {
    heightData[i] = (float *)calloc(params->columns, sizeof(float));
  }

  // Initialize random seed
  srand((unsigned int)time(NULL));

  // Generate hills
  for (int i = 0; i < params->numHills; i++) {
    int hillCenterRow = random_int(0, params->rows - 1);
    int hillCenterCol = random_int(0, params->columns - 1);
    int hillRadius = random_int(params->hillRadiusMin, params->hillRadiusMax);
    float hillHeight =
        random_float(params->hillHeightMin, params->hillHeightMax);

    for (int r = hillCenterRow - hillRadius; r < hillCenterRow + hillRadius;
         r++) {
      for (int c = hillCenterCol - hillRadius; c < hillCenterCol + hillRadius;
           c++) {
        if (r < 0 || r >= params->rows || c < 0 || c >= params->columns) {
          continue;
        }
        int r2 = hillRadius * hillRadius; // r*r term
        int x2x1 = hillCenterCol - c;     // (x2-x1) term
        int y2y1 = hillCenterRow - r;     // (y2-y1) term
        float height = (float)(r2 - x2x1 * x2x1 - y2y1 * y2y1);
        if (height < 0.0f) {
          continue;
        }
        float factor = height / r2;
        heightData[r][c] += hillHeight * factor;
        if (heightData[r][c] > 1.0f) {
          heightData[r][c] = 1.0f;
        }
      }
    }
  }

  return heightData;
}

HeightMap *create_heightmap(HeightData height_data, HillAlgoParams *params) {

  HeightMap *hm = malloc(sizeof(HeightMap));
  hm->heightData = height_data;
  hm->rows = params->rows;
  hm->cols = params->columns;
  hm->numVertices = params->rows * params->columns;

  return hm;
}

void set_vertices(HeightMap *hm) {
  int rows = hm->rows;
  int cols = hm->cols;

  vec3s **vertices = malloc(rows * sizeof(vec3s *));
  for (int i = 0; i < rows; i++) {
    vertices[i] = malloc(cols * sizeof(vec3s));
  }

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      float factorRow = (float)i / (float)(rows - 1);
      float factorCols = (float)j / (float)(cols - 1);
      float height = hm->heightData[i][j];
      vertices[i][j] = VEC3S(-0.5f + factorCols, height, -0.5 + factorRow);
    }
  }
  hm->vertices = vertices;
}

void set_tex_coords(HeightMap *hm) {
  int rows = hm->rows;
  int cols = hm->cols;

  vec2s **coords = malloc(rows * sizeof(vec2s *));
  for (int i = 0; i < rows; i++) {
    coords[i] = malloc(cols * sizeof(vec2s));
  }

  float textureStepU = 0.1f;
  float textureStepV = 0.1f;

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      coords[i][j] = VEC2S(textureStepU * j, textureStepV * i);
    }
  }
  hm->texCoords = coords;
}

void set_normals(HeightMap *hm) {
  int rows = hm->rows;
  int columns = hm->cols;

  // Allocate memory for normals
  hm->normals = (vec3s **)malloc(rows * sizeof(vec3s *));
  for (int i = 0; i < rows; i++) {
    hm->normals[i] = (vec3s *)calloc(columns, sizeof(vec3s));
  }

  // Temporary storage for triangle normals
  vec3s tempNormals[2][rows - 1][columns - 1];

  // Calculate triangle normals
  for (int i = 0; i < rows - 1; i++) {
    for (int j = 0; j < columns - 1; j++) {
      vec3s vertexA = hm->vertices[i][j];
      vec3s vertexB = hm->vertices[i][j + 1];
      vec3s vertexC = hm->vertices[i + 1][j + 1];
      vec3s vertexD = hm->vertices[i + 1][j];

      vec3s edge1, edge2;

      // Triangle A: B-A and A-D
      edge1 = glms_vec3_sub(vertexB, vertexA);
      edge2 = glms_vec3_sub(vertexA, vertexD);
      tempNormals[0][i][j] = glms_vec3_normalize(glms_vec3_cross(edge1, edge2));

      // Triangle B: D-C and C-B
      edge1 = glms_vec3_sub(vertexD, vertexC);
      edge2 = glms_vec3_sub(vertexC, vertexB);
      tempNormals[1][i][j] = glms_vec3_normalize(glms_vec3_cross(edge1, edge2));
    }
  }

  // Calculate vertex normals
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      vec3s finalVertexNormal = GLMS_VEC3_ZERO_INIT;

      int isFirstRow = (i == 0);
      int isFirstColumn = (j == 0);
      int isLastRow = (i == rows - 1);
      int isLastColumn = (j == columns - 1);

      // Upper-left triangle
      if (!isFirstRow && !isFirstColumn) {
        finalVertexNormal =
            glms_vec3_add(finalVertexNormal, tempNormals[0][i - 1][j - 1]);
      }

      // Upper-right triangles
      if (!isFirstRow && !isLastColumn) {
        for (int k = 0; k < 2; k++) {
          finalVertexNormal =
              glms_vec3_add(finalVertexNormal, tempNormals[k][i - 1][j]);
        }
      }

      // Bottom-right triangle
      if (!isLastRow && !isLastColumn) {
        finalVertexNormal =
            glms_vec3_add(finalVertexNormal, tempNormals[0][i][j]);
      }

      // Bottom-left triangles
      if (!isLastRow && !isFirstColumn) {
        for (int k = 0; k < 2; k++) {
          finalVertexNormal =
              glms_vec3_add(finalVertexNormal, tempNormals[k][i][j - 1]);
        }
      }

      // Normalize the final vertex normal and store it
      hm->normals[i][j] = glms_vec3_normalize(finalVertexNormal);
    }
  }
}
