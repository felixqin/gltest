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
    GLint   mPositionLocation;
    GLint   mColorLocation;
};


#endif // CGLPLAYER_H
