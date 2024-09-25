#include <stdio.h>
#include <stdlib.h>
#include "mesh.h"
#include "../array.h"

/******************************************************************/
/**                 R e a d i n g   F i l e                      **/
/******************************************************************/
ModelData modelDataEmpty() {
    ModelData modelData;
    modelData.numVertices = 0;  
    modelData.numFaces = 0;
    modelData.faces = NULL;
    // modelData.transformations = transformationsEmpty(); 
    return modelData;
}
