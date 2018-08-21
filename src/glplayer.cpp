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
    , mTexture(NULL)
    , mIdTexture(0)
    , mTextureLocation(0)
    , mWidth(640)
    , mHeight(360)
    , mImage(NULL)
{
    initImage();
}

CGLPlayer::~CGLPlayer()
{
    free(mImage);

    makeCurrent();
    delete mTexture;
    delete mVShader;
    delete mFShader;
    delete mProgram;
    doneCurrent();
}

void CGLPlayer::initImage()
{
    int w = mWidth;
    int h = mHeight;
    int bytes = w * h * 3;
    mImage = (char*)malloc(bytes);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int r = 255, g = 255, b = 255;
            if (x % 100 != 0 && y % 100 != 0)
            {
                r = (x * y) % 255;
                g = (4 * x) % 255;
                b = (4 * y) % 255;
            }
            int index = (y * w + x) * 3;
            mImage[index+0] = (GLubyte)r;
            mImage[index+1] = (GLubyte)g;
            mImage[index+2] = (GLubyte)b;
        }
    }
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
    // https://www.xuebuyuan.com/2119734.html

    mVShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
            "#version 120\n"
            "attribute vec4 vertexIn;\n"
            "attribute vec2 textureIn;\n"
            "varying vec2 textureOut;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = vertexIn;\n"
            "    textureOut = textureIn;\n"
            "}\n";
    mVShader->compileSourceCode(vsrc);

    mFShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
            "#version 120\n"
            "varying vec2 textureOut;\n"
            "uniform sampler2D rgb;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = texture2D(rgb, textureOut);\n"
            "}\n";
    mFShader->compileSourceCode(fsrc);

    mProgram = new QOpenGLShaderProgram();
    mProgram->addShader(mVShader);
    mProgram->addShader(mFShader);
    mProgram->link();
    mProgram->bind();

    GLuint vetexInLocation = mProgram->attributeLocation("vertexIn");
    GLuint textureInLocation = mProgram->attributeLocation("textureIn");
    mTextureLocation = mProgram->uniformLocation("rgb");

    static const GLfloat vertexVertices[] = {
        // Triangle Strip
        -0.5f, -0.5f,
        0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f
    };

    static const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };

    glVertexAttribPointer(vetexInLocation, 2, GL_FLOAT, false, 0, vertexVertices);
    glEnableVertexAttribArray(vetexInLocation);
    glVertexAttribPointer(textureInLocation, 2, GL_FLOAT, false, 0, textureVertices);
    glEnableVertexAttribArray(textureInLocation);

    mTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    mTexture->create();
    //获取纹理索引值
    mIdTexture = mTexture->textureId();
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mIdTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mImage);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glUniform1i(mTextureLocation, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
