
// shader

char vertex[] = {"\
#version 330\n\
in vec2 vertex_position;\n\
in vec2 vertex_texcoord0;\n\
out vec2 texcoord0;\n\
in vec2 vertex_texcoord1;\n\
out vec2 texcoord1;\n\
in vec2 vertex_texcoord2;\n\
out vec2 texcoord2;\n\
in vec2 vertex_texcoord3;\n\
out vec2 texcoord3;\n\
in vec2 vertex_texcoord4;\n\
out vec2 texcoord4;\n\
in vec2 vertex_texcoord5;\n\
out vec2 texcoord5;\n\
void main() {\n\
gl_Position = vec4(vertex_position, 1.0, 1.0);\n\
texcoord0 = vertex_texcoord0;\n\
texcoord1 = vertex_texcoord1;\n\
texcoord2 = vertex_texcoord2;\n\
texcoord3 = vertex_texcoord3;\n\
texcoord4 = vertex_texcoord4;\n\
texcoord5 = vertex_texcoord5;\n\
}\n"};


char fragment[] = {"\
#version 330\n\
#define texture1D texture\n\
#define texture3D texture\n\
layout(location = 0) out vec4 out_color;\n\
in vec2 texcoord0;\n\
in vec2 texcoord1;\n\
in vec2 texcoord2;\n\
in vec2 texcoord3;\n\
in vec2 texcoord4;\n\
in vec2 texcoord5;\n\
uniform mat3 colormatrix;\n\
uniform vec3 colormatrix_c;\n\
uniform sampler2D texture0;\n\
//uniform vec2 texture_size0;\n\
//uniform mat2 texture_rot0;\n\
//uniform vec2 pixel_size0;\n\
uniform sampler2D texture1;\n\
//uniform vec2 texture_size1;\n\
//uniform mat2 texture_rot1;\n\
//uniform vec2 pixel_size1;\n\
//#define LUT_POS(x, lut_size) mix(0.5 / (lut_size), 1.0 - 0.5 / (lut_size), (x))\n\
void main() {\n\
vec4 color; // = vec4(0.0, 0.0, 0.0, 1.0);\n\
color.r = 1.000000 * vec4(texture(texture0, texcoord0)).r;\n\
color.gb = 1.000000 * vec4(texture(texture1, texcoord1)).rg;\n\
// color conversion\n\
color.rgb = mat3(colormatrix) * color.rgb  + colormatrix_c;\n\
color.a = 1.0;\n\
// color mapping\n\
out_color = color;\n\
}\n"};

char fragment_bt2100[] = {"\
#version 330\n\
#define texture1D texture\n\
#define texture3D texture\n\
layout(location = 0) out vec4 out_color;\n\
in vec2 texcoord0;\n\
in vec2 texcoord1;\n\
in vec2 texcoord2;\n\
in vec2 texcoord3;\n\
in vec2 texcoord4;\n\
in vec2 texcoord5;\n\
uniform mat3 colormatrix;\n\
uniform vec3 colormatrix_c;\n\
uniform mat3 cms_matrix;\n\
uniform sampler2D texture0;\n\
//uniform vec2 texture_size0;\n\
//uniform mat2 texture_rot0;\n\
//uniform vec2 pixel_size0;\n\
uniform sampler2D texture1;\n\
//uniform vec2 texture_size1;\n\
//uniform mat2 texture_rot1;\n\
//uniform vec2 pixel_size1;\n\
//#define LUT_POS(x, lut_size) mix(0.5 / (lut_size), 1.0 - 0.5 / (lut_size), (x))\n\
void main() {\n\
vec4 color; // = vec4(0.0, 0.0, 0.0, 1.0);\n\
color.r = 1.003906 * vec4(texture(texture0, texcoord0)).r;\n\
color.gb = 1.003906 * vec4(texture(texture1, texcoord1)).rg;\n\
// color conversion\n\
color.rgb = mat3(colormatrix) * color.rgb  + colormatrix_c;\n\
color.a = 1.0;\n\
// color mapping\n\
color.rgb = clamp(color.rgb, 0.0, 1.0);\n\
color.rgb = pow(color.rgb, vec3(2.4));\n\
color.rgb = cms_matrix * color.rgb;\n\
color.rgb = clamp(color.rgb, 0.0, 1.0);\n\
color.rgb = pow(color.rgb, vec3(1.0/2.4));\n\
out_color = color;\n\
}\n"};

/* Color conversion matrix: RGB = m * YUV + c
 * m is in row-major matrix, with m[row][col], e.g.:
 *     [ a11 a12 a13 ]     float m[3][3] = { { a11, a12, a13 },
 *     [ a21 a22 a23 ]                       { a21, a22, a23 },
 *     [ a31 a32 a33 ]                       { a31, a32, a33 } };
 * This is accessed as e.g.: m[2-1][1-1] = a21
 * In particular, each row contains all the coefficients for one of R, G, B,
 * while each column contains all the coefficients for one of Y, U, V:
 *     m[r,g,b][y,u,v] = ...
 * The matrix could also be viewed as group of 3 vectors, e.g. the 1st column
 * is the Y vector (1, 1, 1), the 2nd is the U vector, the 3rd the V vector.
 * The matrix might also be used for other conversions and colorspaces.
 */
struct mp_cmat {
    float m[3][3]; // colormatrix
    float c[3];    //colormatrix_c
};

struct mp_mat {
	float m[3][3];
};

// YUV input limited range (16-235 for luma, 16-240 for chroma)
// ITU-R BT.601 (SD)
struct mp_cmat yuv_bt601 = {\
{{ 1.164384, 1.164384,  1.164384 },\
{ 0.00000,  -0.391762,  2.017232 },\
{ 1.596027, -0.812968 , 0.000000 }},\
{-0.874202,  0.531668, -1.085631 } };

// ITU-R BT.709 (HD)
struct mp_cmat yuv_bt709 = {\
{{ 1.164384, 1.164384,  1.164384 },\
{ 0.00000,  -0.213249,  2.112402 },\
{ 1.792741, -0.532909 , 0.000000 }},\
{-0.972945,  0.301483, -1.133402 } };

// ITU-R BT.2020 non-constant luminance system
struct mp_cmat yuv_bt2020ncl = {\
{{ 1.164384, 1.164384,  1.164384 },\
{ 0.00000,  -0.187326,  2.141772 },\
{ 1.678674, -0.650424 , 0.000000 }},\
{-0.915688,  0.347459, -1.148145 } };

// ITU-R BT.2020 constant luminance system
struct mp_cmat yuv_bt2020cl = {\
{{ 0.0000,   1.164384,  0.000000 },\
{ 0.00000,   0.000000,  1.138393 },\
{ 1.138393,  0.000000 , 0.000000 }},\
{-0.571429, -0.073059, -0.571429 } };

float cms_matrix[3][3] = \
{{ 1.660497, -0.124547, -0.018154},\
{-0.587657,  1.132895, -0.100597},\
{-0.072840, -0.008348,  1.118751}};

struct gl_vao_entry {
    // used for shader / glBindAttribLocation
    const char *name;
    // glVertexAttribPointer() arguments
    int num_elems;      // size (number of elements)
    GLenum type;
    bool normalized;
    int offset;
};

struct vertex_pt {
    float x, y;
};

struct vertex_pi {
    GLint x, y;
};

#define TEXUNIT_VIDEO_NUM 6


struct vertex {
    struct vertex_pt position;
    struct vertex_pt texcoord[TEXUNIT_VIDEO_NUM];
};

static const struct gl_vao_entry vertex_vao[] = {
    {"position", 2, GL_FLOAT, false, offsetof(struct vertex, position)},
    {"texcoord0", 2, GL_FLOAT, false, offsetof(struct vertex, texcoord[0])},
    {"texcoord1", 2, GL_FLOAT, false, offsetof(struct vertex, texcoord[1])},
    {"texcoord2", 2, GL_FLOAT, false, offsetof(struct vertex, texcoord[2])},
    {"texcoord3", 2, GL_FLOAT, false, offsetof(struct vertex, texcoord[3])},
    {"texcoord4", 2, GL_FLOAT, false, offsetof(struct vertex, texcoord[4])},
    {"texcoord5", 2, GL_FLOAT, false, offsetof(struct vertex, texcoord[5])},
    {0}
};


static void compile_attach_shader(GLuint program,
                                  GLenum type, const char *source)
{
    GLuint shader;
	GLint status, log_length;
	
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
Debug(3,"compile Status %d loglen %d\n",status,log_length);

    GlxCheck();
    glAttachShader(program, shader);
    glDeleteShader(shader);
}

static void link_shader(GLuint program)
{
	GLint status,log_length;

    glLinkProgram(program);
    status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
Debug(3,"Link Status %d loglen %d\n",status,log_length);


}

static GLuint sc_generate(GLuint gl_prog, enum AVColorSpace colorspace) {

    char vname[80];
    int n;
	GLint cmsLoc;
	float *m,*c,*cms;
	char *frag;
	
	switch (colorspace) {
	case AVCOL_SPC_RGB:
		m = &yuv_bt601.m[0][0];
		c = &yuv_bt601.c[0];
		frag = fragment;
		Debug(3,"BT601 Colorspace used\n");
		break;
	case AVCOL_SPC_BT709:
	case AVCOL_SPC_UNSPECIFIED:   //  comes with UHD
		m = &yuv_bt709.m[0][0];
		c = &yuv_bt709.c[0];
		frag = fragment;
		Debug(3,"BT709 Colorspace used\n");
		break;
	case AVCOL_SPC_BT2020_NCL:
		m = &yuv_bt2020ncl.m[0][0];
		c = &yuv_bt2020ncl.c[0];
		cms = &cms_matrix[0][0];
		frag = fragment_bt2100;
		Debug(3,"BT2020NCL Colorspace used\n");
		break;
	default:								// fallback
		m = &yuv_bt709.m[0][0];
		c = &yuv_bt709.c[0];
		frag = fragment;
		Debug(3,"default BT709 Colorspace used  %d\n",colorspace);
		break;
	}
	
	Debug(3,"vor create\n");
	gl_prog = glCreateProgram();
	Debug(3,"vor compile vertex\n");
	compile_attach_shader(gl_prog, GL_VERTEX_SHADER, vertex);
	Debug(3,"vor compile fragment\n");
	compile_attach_shader(gl_prog, GL_FRAGMENT_SHADER, frag);
	glBindAttribLocation(gl_prog,0,"vertex_position");

	for (n=0;n<6;n++) {
		sprintf(vname,"vertex_texcoord%1d",n);
		glBindAttribLocation(gl_prog,n+1,vname);
	}
		
	link_shader(gl_prog);
	
	gl_colormatrix = glGetUniformLocation(gl_prog,"colormatrix");
	Debug(3,"get uniform colormatrix %d \n",gl_colormatrix);
	if (gl_colormatrix != -1)
		glProgramUniformMatrix3fv(gl_prog,gl_colormatrix,1,0,m);
	GlxCheck();
	  //glProgramUniform3fv(gl_prog,gl_colormatrix,3,&yuv_bt709.m[0][0]);
	gl_colormatrix_c = glGetUniformLocation(gl_prog,"colormatrix_c");
	Debug(3,"get uniform colormatrix_c %d %f\n",gl_colormatrix_c,*c);
	if (gl_colormatrix_c != -1)
	  glProgramUniform3fv(gl_prog,gl_colormatrix_c,1,c);
	GlxCheck();
	
	if (colorspace == AVCOL_SPC_BT2020_NCL) {
		cmsLoc = glGetUniformLocation(gl_prog,"cms_matrix");
		if (cmsLoc != -1)
		  glProgramUniformMatrix3fv(gl_prog,cmsLoc,1,0,cms);
		GlxCheck();
	}
	
    return gl_prog;
}

static void render_pass_quad(int flip, float xcrop, float ycrop)
{
    struct vertex va[4];
    int n;
    const struct gl_vao_entry *e;
	// uhhhh what a hack
	if (!flip ) {
		va[0].position.x = (float) -1.0;
		va[0].position.y = (float)  1.0;
		va[1].position.x = (float) -1.0;
		va[1].position.y = (float) -1.0;
		va[2].position.x = (float)  1.0;
		va[2].position.y = (float)  1.0;
		va[3].position.x = (float)  1.0;
		va[3].position.y = (float) -1.0;
	} else {
		va[0].position.x = (float) -1.0;
		va[0].position.y = (float) -1.0;
		va[1].position.x = (float) -1.0;
		va[1].position.y = (float)  1.0;
		va[2].position.x = (float)  1.0;
		va[2].position.y = (float) -1.0;
		va[3].position.x = (float)  1.0;
		va[3].position.y = (float)  1.0;
	}

    va[0].texcoord[0].x = (float) 0.0 + xcrop;
    va[0].texcoord[0].y = (float) 0.0 + ycrop;  // abgeschnitten von links oben
    va[0].texcoord[1].x = (float) 0.0 + xcrop;
    va[0].texcoord[1].y = (float) 0.0 + ycrop;  // abgeschnitten von links oben
    va[1].texcoord[0].x = (float) 0.0 + xcrop;
    va[1].texcoord[0].y = (float) 1.0 - ycrop;  // abgeschnitten links unten 1.0 - Wert
    va[1].texcoord[1].x = (float) 0.0 + xcrop;
    va[1].texcoord[1].y = (float) 1.0 - ycrop;  // abgeschnitten links unten 1.0 - Wert
    va[2].texcoord[0].x = (float) 1.0 - xcrop;
    va[2].texcoord[0].y = (float) 0.0 + ycrop;  // abgeschnitten von rechts oben
    va[2].texcoord[1].x = (float) 1.0 - xcrop;
    va[2].texcoord[1].y = (float) 0.0 + ycrop;  // abgeschnitten von rechts oben
    va[3].texcoord[0].x = (float) 1.0 - xcrop;
    va[3].texcoord[0].y = (float) 1.0 - ycrop;  // abgeschnitten von rechts unten 1.0 - wert
    va[3].texcoord[1].x = (float) 1.0 - xcrop;
    va[3].texcoord[1].y = (float) 1.0 - ycrop;  // abgeschnitten von rechts unten 1.0 - wert

	
	
    glBindBuffer(GL_ARRAY_BUFFER, vao_buffer);
    glBufferData(GL_ARRAY_BUFFER, 4  * sizeof(struct vertex), va, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	 // enable attribs
    glBindBuffer(GL_ARRAY_BUFFER, vao_buffer);
	for ( n = 0; vertex_vao[n].name; n++) {
        e = &vertex_vao[n];
        glEnableVertexAttribArray(n);
        glVertexAttribPointer(n, e->num_elems, e->type, e->normalized,
                                sizeof(struct vertex), (void *)(intptr_t)e->offset);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// draw quad
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    for ( n = 0; vertex_vao[n].name; n++)
      glDisableVertexAttribArray(n);
}
