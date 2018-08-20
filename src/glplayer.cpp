#include <QPainter>
#include <QOpenGLShader>
#include <QTimerEvent>
#include "glplayer.h"


#define LOGI printf
#define LOGE printf


CGLPlayer::CGLPlayer(QWidget* parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
    , mGLInitialized(false)
    , mProgram(NULL)
    , mVShader(NULL)
    , mFShader(NULL)
{
}

CGLPlayer::~CGLPlayer()
{
    makeCurrent();
    delete mVShader;
    delete mFShader;
    delete mProgram;
    doneCurrent();
}

void CGLPlayer::initializeGL()
{
    initializeOpenGLFunctions();
    initShaders();
    QColor clearColor(Qt::blue);
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    mGLInitialized = true;
}

//Init Shader
void CGLPlayer::initShaders()
{
    // https://blog.csdn.net/su_vast/article/details/52214642
    // https://blog.csdn.net/ylbs110/article/details/52074826

    mVShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
            "#version 120\n"
            "attribute vec4 a_Position;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = a_Position;\n"
            "}\n";
    mVShader->compileSourceCode(vsrc);

    mFShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
            "#version 120\n"
            "uniform vec4 u_Color;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = u_Color;\n"
            "}\n";
    mFShader->compileSourceCode(fsrc);

    mProgram = new QOpenGLShaderProgram();
    mProgram->addShader(mVShader);
    mProgram->addShader(mFShader);
    mProgram->link();
    mProgram->bind();

    mPositionLocation = mProgram->attributeLocation("a_Position");
    mColorLocation = mProgram->uniformLocation("u_Color");

    static const GLfloat vertexData[] = {
        // Triangle 1
        -0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f,

        // Triangle 2
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,

        // Line 1
        0.0f,  7.0f,
        9.0f,  7.0f,

        // Mallets
        4.5f,  2.0f,
        4.5f, 12.0f
    };

    glVertexAttribPointer(mPositionLocation, 2, GL_FLOAT, false, 0, vertexData);
    glEnableVertexAttribArray(mPositionLocation);
}

void CGLPlayer::resizeGL(int w, int h)
{
    if (h == 0)// 防止被零除
    {
        h = 1;// 将高设为1
    }
    //设置视口
    glViewport(0,0, w, h);
}

void CGLPlayer::paintGL()
{
    if (!mGLInitialized)
    {
        LOGE("is not initialized!\n");
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT);

    glUniform4f(mColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
