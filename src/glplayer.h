#ifndef CGLPLAYER_H
#define CGLPLAYER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>


class CGLPlayer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    CGLPlayer(QWidget* parent = 0, Qt::WindowFlags f = 0);

    ~CGLPlayer();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
    void initShaders();

private:
    bool         mGLInitialized;

    QOpenGLShaderProgram* mProgram;
    QOpenGLShader* mVShader;  //顶点着色器程序对象
    QOpenGLShader* mFShader;  //片段着色器对象
    QOpenGLTexture* mTextureY;  //y纹理对象
    QOpenGLTexture* mTextureU;  //u纹理对象
    QOpenGLTexture* mTextureV;  //v纹理对象
    GLint                 mTextureUniformY;
    GLint                 mTextureUniformU;
    GLint                 mTextureUniformV;
    GLuint                mIdY;         // Texture id
    GLuint                mIdU;         // Texture id
    GLuint                mIdV;         // Texture id
};


#endif // CGLPLAYER_H
