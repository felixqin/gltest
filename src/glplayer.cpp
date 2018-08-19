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
    , mTextureY(NULL)
    , mTextureU(NULL)
    , mTextureV(NULL)
{
}

CGLPlayer::~CGLPlayer()
{
    makeCurrent();
    delete mTextureY;
    delete mTextureU;
    delete mTextureV;
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
            "varying vec2 textureOut;\n"
            "uniform sampler2D tex_y;\n"
            "uniform sampler2D tex_u;\n"
            "uniform sampler2D tex_v;\n"
            "void main(void)\n"
            "{\n"
            "    vec3 yuv;                                      \n"
            "    vec3 rgb;                                      \n"
            "    yuv.x = texture2D(tex_y, textureOut).r;        \n"
            "    yuv.y = texture2D(tex_u, textureOut).r - 0.5;  \n"
            "    yuv.z = texture2D(tex_v, textureOut).r - 0.5;  \n"
            "    rgb = mat3( 1,       1,         1,             \n"
            "                0,       -0.39465,  2.03211,       \n"
            "                1.13983, -0.58060,  0) * yuv;      \n"
            "    gl_FragColor = vec4(rgb, 1);                   \n"
            "}\n";
    mFShader->compileSourceCode(fsrc);

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
    mProgram = new QOpenGLShaderProgram();
    mProgram->addShader(mVShader);
    mProgram->addShader(mFShader);
    mProgram->bindAttributeLocation("vertexIn", ATTRIB_VERTEX);
    mProgram->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
    mProgram->link();
    mProgram->bind();

    //Get Uniform Variables Location
    mTextureUniformY = mProgram->uniformLocation("tex_y");
    mTextureUniformU = mProgram->uniformLocation("tex_u");
    mTextureUniformV = mProgram->uniformLocation("tex_v");

    static const GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };

    static const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };

    //设置属性ATTRIB_VERTEX的顶点矩阵值以及格式
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
    //设置属性ATTRIB_TEXTURE的纹理矩阵值以及格式
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
    //启用ATTRIB_VERTEX属性的数据,默认是关闭的
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    //启用ATTRIB_TEXTURE属性的数据,默认是关闭的
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    //分别创建y,u,v纹理对象
    mTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    mTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    mTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
    mTextureY->create();
    mTextureU->create();
    mTextureV->create();
    //获取返回y分量的纹理索引值
    mIdY = mTextureY->textureId();
    //获取返回u分量的纹理索引值
    mIdU = mTextureU->textureId();
    //获取返回v分量的纹理索引值
    mIdV = mTextureV->textureId();
}

void CGLPlayer::resizeGL(int w, int h)
{
    if (h == 0)// 防止被零除
    {
        h = 1;// 将高设为1
    }
    //设置视口
    glViewport(0,0, w,h);
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

#if 0
    int y_w = info->image.width;
    int y_h = info->image.height;

    int uv_w = y_w / 2;
    int uv_h = y_h / 2;
    if (info->image.format == stream::IMAGE_FORMAT_I422)
    {
        uv_h = y_h;
    }

    //Y
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mIdY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, y_w, y_h, 0, GL_RED, GL_UNSIGNED_BYTE, frame.data(0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //U
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mIdU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, uv_w, uv_h, 0, GL_RED, GL_UNSIGNED_BYTE, frame.data(1));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //V
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mIdV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, uv_w, uv_h, 0, GL_RED, GL_UNSIGNED_BYTE, frame.data(2));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //指定y纹理要使用新值 只能用0,1,2等表示纹理单元的索引，这是opengl不人性化的地方
    //0对应纹理单元GL_TEXTURE0 1对应纹理单元GL_TEXTURE1 2对应纹理的单元
    glUniform1i(mTextureUniformY, 0);
    glUniform1i(mTextureUniformU, 1);
    glUniform1i(mTextureUniformV, 2);

    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif

#if 0
    QPainter pter(this);
    pter.setPen(Qt::blue);
    pter.drawText(20, 50, "This is a Text!");
    pter.drawText(20, 100, ptsstr.c_str());
    pter.end();
#endif
}
