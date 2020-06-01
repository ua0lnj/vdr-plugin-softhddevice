#ifndef __SOFTHDDEVICE_OPENGLOSD_H
#define __SOFTHDDEVICE_OPENGLOSD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_STROKER_H

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
    int          code;
    const char*  message;
} FT_Errors[] =
#include FT_ERRORS_H


#include <memory>
#include <queue>

//#include <vdr/plugin.h>
#include <vdr/osd.h>
//#include <vdr/thread.h>

#include "softhddev.h"

extern "C"
{
#include <stdint.h>
#include <libavcodec/avcodec.h>

#include "audio.h"
//#include "video.h"
//#include "codec.h"
}

struct sOglImage {
    GLuint texture;
    GLint width;
    GLint height;
    bool used;
};

/****************************************************************************************
* Helpers
****************************************************************************************/

void ConvertColor(const GLint &colARGB, glm::vec4 &col);

/****************************************************************************************
* cShader
****************************************************************************************/
enum eShaderType {
    stRect,
    stTexture,
    stText,
    stCount
};

class cShader {
private:
    eShaderType type;
    GLuint id;
    bool Compile(const char *vertexCode, const char *fragmentCode);
    bool CheckCompileErrors(GLuint object, bool program = false);
public:
    cShader(void) {};
    virtual ~cShader(void) {};
    bool Load(eShaderType type);
    void Use(void);
    void SetFloat    (const GLchar *name, GLfloat value);
    void SetInteger  (const GLchar *name, GLint value);
    void SetVector2f (const GLchar *name, GLfloat x, GLfloat y);
    void SetVector3f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
    void SetVector4f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void SetMatrix4  (const GLchar *name, const glm::mat4 &matrix);
};

/****************************************************************************************
* cOglGlyph
****************************************************************************************/
class cOglGlyph : public cListObject {
private:
    struct tKerning {
        public:
            tKerning(uint prevSym, GLfloat kerning = 0.0f)  {
                this->prevSym = prevSym;
                this->kerning = kerning;
            }
            uint prevSym;
            GLfloat kerning;
    };
    uint charCode;
    int bearingLeft;
    int bearingTop;
    int width;
    int height;
    int advanceX;      
    cVector<tKerning> kerningCache;
    GLuint texture;
    void LoadTexture(FT_BitmapGlyph ftGlyph);
public:
    cOglGlyph(uint charCode, FT_BitmapGlyph ftGlyph);
    virtual ~cOglGlyph();
    uint CharCode(void) { return charCode; }
    int AdvanceX(void) { return advanceX; }
    int BearingLeft(void) const { return bearingLeft; }
    int BearingTop(void) const { return bearingTop; }
    int Width(void) const { return width; }
    int Height(void) const { return height; }
    int GetKerningCache(uint prevSym);
    void SetKerningCache(uint prevSym, int kerning);
    void BindTexture(void);
};

/****************************************************************************************
* cOglFont
****************************************************************************************/
class cOglFont : public cListObject {
private:
    static bool initiated;
    cString name;
    int size;
    int height;
    int bottom;
    static FT_Library ftLib;
    FT_Face face;
    static cList<cOglFont> *fonts;
    mutable cList<cOglGlyph> glyphCache;
    cOglFont(const char *fontName, int charHeight);
    static void Init(void);
public:
    virtual ~cOglFont(void);
    static cOglFont *Get(const char *name, int charHeight);
    static void Cleanup(void);
    const char *Name(void) { return *name; };
    int Size(void) { return size; };
    int Bottom(void) {return bottom; };
    int Height(void) {return height; };
    cOglGlyph* Glyph(uint charCode) const;
    int Kerning(cOglGlyph *glyph, uint prevSym) const;
};

/****************************************************************************************
* cOglFb
* Framebuffer Object - OpenGL part of a Pixmap
****************************************************************************************/
class cOglFb {
protected:
    bool initiated;
    GLuint fb;
    GLuint texture;
    GLint width, height;
    GLint viewPortWidth, viewPortHeight;
    bool scrollable;
public:
    cOglFb(GLint width, GLint height, GLint viewPortWidth, GLint viewPortHeight);
    virtual ~cOglFb(void);
    bool Initiated(void) { return initiated; }
    virtual bool Init(void);
    void Bind(void);
    void BindRead(void);
    virtual void BindWrite(void);
    virtual void Unbind(void);
    bool BindTexture(void);
    void Blit(GLint destX1, GLint destY1, GLint destX2, GLint destY2);
    GLint Width(void) { return width; };
    GLint Height(void) { return height; };
    bool Scrollable(void) { return scrollable; };
    GLint ViewportWidth(void) { return viewPortWidth; };
    GLint ViewportHeight(void) { return viewPortHeight; };
};

/****************************************************************************************
* cOglOutputFb
* Output Framebuffer Object - holds Vdpau Output Surface which is our "output framebuffer"
****************************************************************************************/
class cOglOutputFb : public cOglFb {
private:
    GLvdpauSurfaceNV surface;
bool run;
public:
    cOglOutputFb(GLint width, GLint height);
    virtual ~cOglOutputFb(void);
    virtual bool Init(void);
    virtual void BindWrite(void);
    virtual void Unbind(void);
    virtual bool Run(){return run;}
};

/****************************************************************************************
* cOglVb
* Vertex Buffer - OpenGl Vertices for the different drawing commands  
****************************************************************************************/
enum eVertexBufferType {
    vbRect,
    vbEllipse,
    vbSlope,
    vbTexture,
    vbText,
    vbCount
};

class cOglVb {
private:
    eVertexBufferType type;
    eShaderType shader;
    GLuint vao;
    GLuint vbo;
    int sizeVertex1;
    int sizeVertex2;
    int numVertices;
    GLuint drawMode;
public:
    cOglVb(int type);
    virtual ~cOglVb(void);
    bool Init(void);
    void Bind(void);
    void Unbind(void);
    void ActivateShader(void);
    void EnableBlending(void);
    void DisableBlending(void);
    void SetShaderColor(GLint color);
    void SetShaderAlpha(GLint alpha);
    void SetShaderProjectionMatrix(GLint width, GLint height);
    void SetVertexData(GLfloat *vertices, int count = 0);
    void DrawArrays(int count = 0);
};

/****************************************************************************************
* cOpenGLCmd
****************************************************************************************/
class cOglCmd {
protected:
    cOglFb *fb;
public:
    cOglCmd(cOglFb *fb) { this->fb = fb; };
    virtual ~cOglCmd(void) {};
    virtual const char* Description(void) = 0;
    virtual bool Execute(void) = 0;
};

class cOglCmdInitOutputFb : public cOglCmd {
private:
    cOglOutputFb *oFb;
public:
    cOglCmdInitOutputFb(cOglOutputFb *oFb);
    virtual ~cOglCmdInitOutputFb(void) {};
    virtual const char* Description(void) { return "InitOutputFramebuffer"; }
    virtual bool Execute(void);
};

class cOglCmdInitFb : public cOglCmd {
private:
    cCondWait *wait;
public:
    cOglCmdInitFb(cOglFb *fb, cCondWait *wait = NULL);
    virtual ~cOglCmdInitFb(void) {};
    virtual const char* Description(void) { return "InitFramebuffer"; }
    virtual bool Execute(void);
};

class cOglCmdDeleteFb : public cOglCmd {
public:
    cOglCmdDeleteFb(cOglFb *fb);
    virtual ~cOglCmdDeleteFb(void) {};
    virtual const char* Description(void) { return "DeleteFramebuffer"; }
    virtual bool Execute(void);
};

class cOglCmdRenderFbToBufferFb : public cOglCmd {
private:
    cOglFb *buffer;
    GLfloat x, y;
    GLfloat drawPortX, drawPortY;
    GLint transparency;
public:
    cOglCmdRenderFbToBufferFb(cOglFb *fb, cOglFb *buffer, GLint x, GLint y, GLint transparency, GLint drawPortX, GLint drawPortY);
    virtual ~cOglCmdRenderFbToBufferFb(void) {};
    virtual const char* Description(void) { return "Render Framebuffer to Buffer"; }
    virtual bool Execute(void);
};

class cOglCmdCopyBufferToOutputFb : public cOglCmd {
private:
    cOglOutputFb *oFb;
    GLint x, y;
public:
    cOglCmdCopyBufferToOutputFb(cOglFb *fb, cOglOutputFb *oFb, GLint x, GLint y);
    virtual ~cOglCmdCopyBufferToOutputFb(void) {};
    virtual const char* Description(void) { return "Copy buffer to OutputFramebuffer"; }
    virtual bool Execute(void);
};

class cOglCmdFill : public cOglCmd {
private:
    GLint color;
public:
    cOglCmdFill(cOglFb *fb, GLint color);
    virtual ~cOglCmdFill(void) {};
    virtual const char* Description(void) { return "Fill"; }
    virtual bool Execute(void);
};

class cOglCmdDrawRectangle : public cOglCmd {
private:
    GLint x, y;
    GLint width, height;
    GLint color;
public:
    cOglCmdDrawRectangle(cOglFb *fb, GLint x, GLint y, GLint width, GLint height, GLint color);
    virtual ~cOglCmdDrawRectangle(void) {};
    virtual const char* Description(void) { return "DrawRectangle"; }
    virtual bool Execute(void);
};

class cOglCmdDrawEllipse : public cOglCmd {
private:
    GLint x, y;
    GLint width, height;
    GLint color;
    GLint quadrants;
    GLfloat *CreateVerticesFull(int &numVertices);
    GLfloat *CreateVerticesQuadrant(int &numVertices);
    GLfloat *CreateVerticesHalf(int &numVertices); 
public:
    cOglCmdDrawEllipse(cOglFb *fb, GLint x, GLint y, GLint width, GLint height, GLint color, GLint quadrants);
    virtual ~cOglCmdDrawEllipse(void) {};
    virtual const char* Description(void) { return "DrawEllipse"; }
    virtual bool Execute(void);
};

class cOglCmdDrawSlope : public cOglCmd {
private:
    GLint x, y;
    GLint width, height;
    GLint color;
    GLint type;
public:
    cOglCmdDrawSlope(cOglFb *fb, GLint x, GLint y, GLint width, GLint height, GLint color, GLint type);
    virtual ~cOglCmdDrawSlope(void) {};
    virtual const char* Description(void) { return "DrawSlope"; }
    virtual bool Execute(void);
};

class cOglCmdDrawText : public cOglCmd {
private:
    GLint x, y;
    GLint limitX;
    GLint colorText;
    cString fontName;
    int fontSize;
    unsigned int *symbols;
public:
    cOglCmdDrawText(cOglFb *fb, GLint x, GLint y, unsigned int *symbols, GLint limitX, const char *name, int fontSize, tColor colorText);
    virtual ~cOglCmdDrawText(void);
    virtual const char* Description(void) { return "DrawText"; }
    virtual bool Execute(void);
};

class cOglCmdDrawImage : public cOglCmd {
private:
    tColor *argb;
    GLint x, y, width, height;
    bool overlay;
    GLfloat scaleX, scaleY;
public:
    cOglCmdDrawImage(cOglFb *fb, tColor *argb, GLint width, GLint height, GLint x, GLint y, bool overlay = true, double scaleX = 1.0f, double scaleY = 1.0f);
    virtual ~cOglCmdDrawImage(void);
    virtual const char* Description(void) { return "Draw Image"; }
    virtual bool Execute(void);
};

class cOglCmdDrawTexture : public cOglCmd {
private:
    sOglImage *imageRef;
    GLint x, y;
public:
    cOglCmdDrawTexture(cOglFb *fb, sOglImage *imageRef, GLint x, GLint y);
    virtual ~cOglCmdDrawTexture(void) {};
    virtual const char* Description(void) { return "Draw Texture"; }
    virtual bool Execute(void);
};

class cOglCmdStoreImage : public cOglCmd {
private:
    sOglImage *imageRef;
    tColor *data;
public:
    cOglCmdStoreImage(sOglImage *imageRef, tColor *argb);
    virtual ~cOglCmdStoreImage(void);
    virtual const char* Description(void) { return "Store Image"; }
    virtual bool Execute(void);
};

class cOglCmdDropImage : public cOglCmd {
private:
    sOglImage *imageRef;
    cCondWait *wait;
public:
    cOglCmdDropImage(sOglImage *imageRef, cCondWait *wait);
    virtual ~cOglCmdDropImage(void) {};
    virtual const char* Description(void) { return "Drop Image"; }
    virtual bool Execute(void);
};

/******************************************************************************
* cOglThread
******************************************************************************/
#define OGL_MAX_OSDIMAGES 256
#define OGL_CMDQUEUE_SIZE 100

class cOglThread : public cThread {
private:
    cCondWait *startWait;
    cCondWait *wait;
    bool stalled;
    std::queue<cOglCmd*> commands;
    GLint maxTextureSize;
    sOglImage imageCache[OGL_MAX_OSDIMAGES];
    long memCached;
    long maxCacheSize;
    bool InitOpenGL(void);
    bool InitShaders(void);
    void DeleteShaders(void);
    bool InitVdpauInterop(void);
    bool InitVertexBuffers(void);
    void DeleteVertexBuffers(void);
    void Cleanup(void);
    int GetFreeSlot(void);
    void ClearSlot(int slot);
protected:
    virtual void Action(void);
public:
    cOglThread(cCondWait *startWait, int maxCacheSize);
    virtual ~cOglThread();
    void Stop(void);
    void DoCmd(cOglCmd* cmd);
    int StoreImage(const cImage &image);
    void DropImageData(int imageHandle);
    sOglImage *GetImageRef(int slot);
    int MaxTextureSize(void) { return maxTextureSize; };
};

/****************************************************************************************
* cOglPixmap
****************************************************************************************/
class cOglPixmap : public cPixmap {
private:
    cOglFb *fb;
    std::shared_ptr<cOglThread> oglThread;
    bool dirty;
public:
    cOglPixmap(std::shared_ptr<cOglThread> oglThread, int Layer, const cRect &ViewPort, const cRect &DrawPort = cRect::Null);
    virtual ~cOglPixmap(void);
    cOglFb *Fb(void) { return fb; };
    int X(void) { return ViewPort().X(); };
    int Y(void) { return ViewPort().Y(); };
    virtual bool IsDirty(void) { return dirty; }
    virtual void SetDirty(bool dirty = true) { this->dirty = dirty; }
    virtual void SetAlpha(int Alpha);
    virtual void SetTile(bool Tile);
    virtual void SetViewPort(const cRect &Rect);
    virtual void SetDrawPortPoint(const cPoint &Point, bool Dirty = true);
    virtual void Clear(void);
    virtual void Fill(tColor Color);
    virtual void DrawImage(const cPoint &Point, const cImage &Image);
    virtual void DrawImage(const cPoint &Point, int ImageHandle);
    virtual void DrawPixel(const cPoint &Point, tColor Color);
    virtual void DrawBitmap(const cPoint &Point, const cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0, bool Overlay = false);
    virtual void DrawText(const cPoint &Point, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width = 0, int Height = 0, int Alignment = taDefault);
    virtual void DrawRectangle(const cRect &Rect, tColor Color);
    virtual void DrawEllipse(const cRect &Rect, tColor Color, int Quadrants = 0);
    virtual void DrawSlope(const cRect &Rect, tColor Color, int Type);
    virtual void Render(const cPixmap *Pixmap, const cRect &Source, const cPoint &Dest);
    virtual void Copy(const cPixmap *Pixmap, const cRect &Source, const cPoint &Dest);
    virtual void Scroll(const cPoint &Dest, const cRect &Source = cRect::Null);
    virtual void Pan(const cPoint &Dest, const cRect &Source = cRect::Null);
};

/******************************************************************************
* cOglOsd
******************************************************************************/
class cOglOsd : public cOsd {
private:
    cOglFb *bFb;
    std::shared_ptr<cOglThread> oglThread;
    cVector<cOglPixmap *> oglPixmaps;
    bool isSubtitleOsd;
protected:
public:
    cOglOsd(int Left, int Top, uint Level, std::shared_ptr<cOglThread> oglThread);
    virtual ~cOglOsd();
    virtual eOsdError SetAreas(const tArea *Areas, int NumAreas);
    virtual cPixmap *CreatePixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort = cRect::Null);
    virtual void DestroyPixmap(cPixmap *Pixmap);
    virtual void Flush(void);
    virtual void DrawScaledBitmap(int x, int y, const cBitmap &Bitmap, double FactorX, double FactorY, bool AntiAlias = false);
    static cOglOutputFb *oFb;
};

#endif //__SOFTHDDEVICE_OPENGLOSD_H