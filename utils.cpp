#include <utils.h>

int LINE_LENGTH=100;

MeshObject::MeshObject(const char * file_name) {
        //BEGIN PLY FILE HANDLING
    FILE *ply;

    char line[LINE_LENGTH]; //line to be read
    amount_of_vertices = 0;
    amount_of_faces = 0;

    ply = fopen (file_name, "rt"); //open ply file

    //read header
    while (fgets(line, LINE_LENGTH, ply) != NULL)
    {
        /* convert the string to a long int */
        if (!strncmp (line, "element vertex", 14)) {
            sscanf(line, "element vertex %i", &amount_of_vertices);

        } else if (!strncmp (line, "element face", 12)) {
            sscanf(line, "element face %i", &amount_of_faces);
        }

        else if (!strncmp(line, "end_header", 10)) {
            break;
        }
    }


    #ifdef DEBUG
    printf("file format says %i vertices, %i faces\n", 
        amount_of_vertices,
        amount_of_faces);
    #endif
  
    //read vertices
    int i;
    float x;
    float y;
    float z;

    //create vertex table for pushing into GLSL
    vertices = (float *) malloc(amount_of_vertices * 3 * sizeof(float));
    tex_coordinates = (float *) malloc(amount_of_vertices * 2 *
    sizeof(float)); //only u and v for texture 
    
    //set the vertex coordinates into vertices
    for(i = 0; i < amount_of_vertices; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            vertices[(3*i)] = x;
            vertices[(3*i)+1] = y;
            vertices[(3*i)+2] = z;
            //trivial x=u, y=v mapping
            tex_coordinates[2*i] = x;
            tex_coordinates[2*i+1] = y;
        }
    }

    //the indices of the faces is needed for what?
    faces_indices = (int *) malloc(amount_of_faces * 3 * sizeof(int));
    
    //we need the face normal to be able to shade the faces correclty
    faces_normals = (float *) malloc(amount_of_faces * 3 * sizeof(float));
    
    //the vertex normals
    vertex_normals = (float *) malloc(amount_of_vertices * 3 * sizeof(float));
    
    
    //count the face normals
    int amount_of_face_indices;
    int a, b, c;
    int * amount_of_faces_per_vertex = (int *) malloc(amount_of_vertices * sizeof(int));
    for(i = 0; i < amount_of_vertices; i++) {
        amount_of_faces_per_vertex[i] = 0;
    }
    
    bool * visited = (bool*) malloc(amount_of_faces * sizeof(bool*));
    for(i = 0; i < amount_of_faces; i++) {
        visited[i] = false;
    }

    int ** vertex_in_faces =(int**) malloc(amount_of_vertices * sizeof(int*));
    for (i = 0; i < amount_of_vertices; i++){
        vertex_in_faces[i] = (int *)malloc(9 * sizeof(int)); //hope this is enough
        int j;
        for (j = 0; j < 9; j++) {
            vertex_in_faces[i][j] = -1;
        }
    }
    //todoo, free vertex_in_faces and visited after they're not needed
    
    
    for(i = 0; i < amount_of_faces; i++) {
        fscanf (ply, "%i %i %i %i", &amount_of_face_indices, &a, &b, &c);
        if (amount_of_face_indices == 3) //we're only interested in triangels
        {
            //save the index of each vertex
            faces_indices[3*i] = a;      
            faces_indices[(3*i)+1] = b;      
            faces_indices[(3*i)+2] = c;
            
            vertex_in_faces[a][amount_of_faces_per_vertex[a]] = i; 
            vertex_in_faces[b][amount_of_faces_per_vertex[b]] = i; 
            vertex_in_faces[c][amount_of_faces_per_vertex[c]] = i; 

            amount_of_faces_per_vertex[a]++;
            amount_of_faces_per_vertex[b]++;
            amount_of_faces_per_vertex[c]++;

        }
         else {
            fprintf(stderr, "error, bad index data: not triangle!");    
        }
    }
    
   //just choose the face index where to start recursive calculation
   float reference[3];
   lazy_calc_normal(faces_indices[3*vertex_in_faces[0][0]],
                    faces_indices[3*vertex_in_faces[0][0]+1], 
                    faces_indices[3*vertex_in_faces[0][0]+2],
                    reference); 

    //count the face normals
    for(i = 0; i < amount_of_faces; i++) {
            a = faces_indices[3*i];      
            b = faces_indices[(3*i)+1];      
            c = faces_indices[(3*i)+2];
            //point vectors to the vertices for the triangle
            float v1[3] = {vertices[(a * 3)], 
                            vertices[(a * 3) + 1], 
                            vertices[(a * 3) + 2]};
            float v2[3] = {vertices[(b * 3)], 
                            vertices[(b * 3) + 1], 
                            vertices[(b * 3) + 2]};
            float v3[3] = {vertices[(c * 3)], 
                            vertices[(c * 3) + 1], 
                            vertices[(c * 3) + 2]};

            //calculate the face normal
            float res[3];
            calc_normal(v1, v2, v3, res);

            // add the face normal to bunny
            faces_normals[(i * 3)] = res[0];
            faces_normals[(i * 3) + 1] = res[1];
            faces_normals[(i * 3) + 2] = res[2];
            
    }
    
    //check the faces_normals just calculated and turn them around
    //if needed
        
    //choose a reference face (index of the reference face)
    int reference_face_index = 0;
    recursive_orient(reference_face_index, 
                    faces_normals,
                    faces_indices,
                    vertex_in_faces, 
                    visited);
    
    //add the faces normals to the vertex normals
    for(i = 0; i < amount_of_faces; i++) {
            a = faces_indices[3*i];      
            b = faces_indices[(3*i)+1];      
            c = faces_indices[(3*i)+2];
            
            
            float res[3] = {
                faces_normals[(i * 3)],
                faces_normals[(i * 3) + 1],
                faces_normals[(i * 3) + 2]
            };
            
           //(only add, normalize later)
            vertex_normals[(a * 3)] += res[0];
            vertex_normals[(a * 3) + 1] += res[1];
            vertex_normals[(a * 3) + 2] += res[2];
            
            vertex_normals[(b * 3)] += res[0];
            vertex_normals[(b * 3) + 1] += res[1];
            vertex_normals[(b * 3) + 2] += res[2];

            vertex_normals[(c * 3)] += res[0];
            vertex_normals[(c * 3) + 1] += res[1];
            vertex_normals[(c * 3) + 2] += res[2];
    }
    
    //normalize the vertex normals (average)
    for(i = 0; i < amount_of_vertices; i++) {
        float temp[3];
        temp[0] = vertex_normals[(i * 3)];
        temp[1] = vertex_normals[(i * 3) + 1];
        temp[2] = vertex_normals[(i * 3) + 2]; 
        
        float normalization = sqrt(temp[0] * temp[0] + temp[1] * temp[1] + temp[2] * temp[2]);
        temp[0] = temp[0]/normalization;
        temp[1] = temp[1]/normalization;
        temp[2] = temp[2]/normalization;

        vertex_normals[(i * 3)] = temp[0];
        vertex_normals[(i * 3) + 1] = temp[1];
        vertex_normals[(i * 3) + 2] = temp[2];
        
    }

    fclose(ply); //close file

    //END PLY FILE HANDLING

}

void MeshObject::calc_normal(float * first, float * second, float * third, float * returnme)
{

    
    float v1[3];
    float v2[3];
    float *r = returnme;
    float normalization;
    
    //compute vectors
    v1[0] = first[0] - second[0];
    v1[1] = first[1] - second[1];
    v1[2] = first[2] - second[2];

    v2[0] = first[0] - third[0];
    v2[1] = first[1] - third[1];
    v2[2] = first[2] - third[2];

    //cross product
    r[0] = v1[1]*v2[2]- v2[1]*v1[2];
    r[1] = v2[0]*v1[2] - v1[0]*v2[2];
    r[2] = v1[0]*v2[1]- v2[0]*v1[1];

    // just calculate the normal the normalization should happen elsewhere
    // the area of the triangle is important to weight the normal compared
    // to other normals (?)
    normalization = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
    r[0] = r[0]/normalization;
    r[1] = r[1]/normalization;
    r[2] = r[2]/normalization;

}


void MeshObject::lazy_calc_normal(int a, int b, int c, float*ret){
             //point vectors to the vertices for the triangle
            float v1[3] = {vertices[(a * 3)], 
                            vertices[(a * 3) + 1], 
                            vertices[(a * 3) + 2]};
            float v2[3] = {vertices[(b * 3)], 
                            vertices[(b * 3) + 1], 
                            vertices[(b * 3) + 2]};
            float v3[3] = {vertices[(c * 3)], 
                            vertices[(c * 3) + 1], 
                            vertices[(c * 3) + 2]};

            calc_normal(v1, v2, v3, ret); 
    }

float MeshObject::calc_dot_product(float * v1, float * v2) 
{
    return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}



void MeshObject::recursive_orient(int face_index, 
                    float * faces_normals, 
                    int * faces_indices, 
                    int ** vertex_in_faces,
                    bool * visited) {
    
              
    if(visited[face_index])
        return;
        
    //mark this face as visited
    visited[face_index] = true;
    
    //current_face normal
    float current_face_normal[3] = {
        faces_normals[(face_index * 3)],
        faces_normals[(face_index * 3) + 1],
        faces_normals[(face_index * 3) + 2]
    };
    
    //vertex indices that is connected to the current_face
    int vi1 = faces_indices[(face_index * 3)];
    int vi2 = faces_indices[(face_index * 3) + 1];
    int vi3 = faces_indices[(face_index * 3) + 2];
    
    int i;
    for(i = 0; i < 9; i++) {
        //the face indexes that is connected to the current_face
        int fi1 = vertex_in_faces[vi1][i];
        int fi2 = vertex_in_faces[vi2][i];
        int fi3 = vertex_in_faces[vi3][i];
        
        if(fi1 != -1 && fi1 != face_index && !visited[fi1]) {
            float nface_normal[3] = {
                faces_normals[(fi1 * 3)],
                faces_normals[(fi1 * 3) + 1],
                faces_normals[(fi1 * 3) + 2]  
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {
                
                faces_normals[(fi1 * 3)] = -1 * faces_normals[(fi1 * 3)];
                faces_normals[(fi1 * 3) + 1] = -1 * faces_normals[(fi1 * 3) + 1];
                faces_normals[(fi1 * 3) + 2] = -1 * faces_normals[(fi1 * 3) + 2];
            }
        }
        if(fi2 != -1 && fi2 != face_index && !visited[fi1]) {
            float nface_normal[3] = {
                faces_normals[(fi2 * 3)],
                faces_normals[(fi2 * 3) + 1],
                faces_normals[(fi2 * 3) + 2]
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {
                faces_normals[(fi2 * 3)] = -1 * faces_normals[(fi2 * 3)];
                faces_normals[(fi2 * 3) + 1] = -1 * faces_normals[(fi2 * 3) + 1];
                faces_normals[(fi2 * 3) + 2] = -1 * faces_normals[(fi2 * 3) + 2];
            }
        }
        if(fi3 != -1 && fi3 != face_index && !visited[fi1]) {
            float nface_normal[3] = {
                faces_normals[(fi3 * 3)],
                faces_normals[(fi3 * 3) + 1],
                faces_normals[(fi3 * 3) + 2]
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {  
                faces_normals[(fi3 * 3)] = -faces_normals[(fi3 * 3)];
                faces_normals[(fi3 * 3) + 1] = -faces_normals[(fi3 * 3) + 1];
                faces_normals[(fi3 * 3) + 2] = -faces_normals[(fi3 * 3) + 2];
            }
        }
        if(fi1 != -1) {
            recursive_orient(fi1, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
        if(fi2 != -1) {           
            recursive_orient(fi2, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
        if(fi3 != -1) {    
            recursive_orient(fi3, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
    }
}

