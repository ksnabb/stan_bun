#define DEBUG

//describes the information in a ply file
typedef struct {
    float * vertices;
    int amount_of_vertices;
    int * faces_indices;
    float * faces_normals;
    int amount_of_faces;
    float * vertex_normals;
    float * tex_coordinates; //1 per vertex
} ply_object;

typedef struct {
    float fovy;
    float aspect;
    float near;
    float far;
    int width;
    int height;

} projection_data;

typedef struct {
    float x;
    float y;
    float z;
} scale;

typedef struct {
    float camera_x;
    float camera_y;
    float camera_z;
    float lookat_x;
    float lookat_y;
    float lookat_z;
    float up_x;
    float up_y;
    float up_z;
    float angle;

} modelview_data;

typedef struct {
    int width;
    int height;
    GLchar * whole;
    GLchar * rgbstart;

} img_data;

typedef struct {
    float dx;
    float dy;
    float dz;
    float nx;
    float ny;
    float nz;
    
} vertex_data;

//util.c
GLchar * simple_fileread(char * file_name, GLint * length);
ply_object read_ply_from_file(const char *file_name);

//glew_stuff.c

void display_cb(void);
void keyb_cb(unsigned char key, int x, int y);
void sp_keyb_cb(int key, int x, int y);
void mouse_motion_cb(int x, int y);
void mouse_cb(int button, int state, int x, int y);
void mouse_cb(int button, int state, int x, int y);
void timer_cb(int unused);
void window_resize_cb(int width, int height);
void update_modelview();
void update_projection();
img_data * simple_bmp_read(char * file_name);
