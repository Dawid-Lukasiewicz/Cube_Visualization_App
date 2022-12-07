// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QTextStream>
#include <math.h>

bool GLWidget::m_transparent = false;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }


    socket =  std::make_shared<QTcpSocket>(this);

    //Connect signal to glWidget and to this window
    connect( socket.get(), SIGNAL(connected()), this, SLOT(connected()) );
    //Disconnect signal to glWidget and to this window
    connect( socket.get(), SIGNAL(disconnected()), this, SLOT(disconnected()) );
    //readRead signal to glWidget
    connect( socket.get(), SIGNAL(readyRead()), this, SLOT(readyRead()) );
    //bytesWritten signal to glWidget
    connect( socket.get(), SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)) );

    /* >>>>>>>>>Initialize socket (move it later to connect button)<<<<<<<<<<< */
    socket->connectToHost("192.168.100.13", 1234);
    qDebug() << "Connecting...";

    if (socket->state() != QAbstractSocket::ConnectedState
            && !socket->waitForConnected(1000))
    {
        qDebug() << "Error while connecting: " << socket->error() << "\n";
    }
    /* >>>>>>>>>Initialize socket (move it later to connect button)<<<<<<<<<<< */
}

GLWidget::~GLWidget()
{
    socket->disconnectFromHost();
    if (socket->state() != QAbstractSocket::UnconnectedState
            && !socket->waitForDisconnected())
    {
        qDebug() << "Error while disconnecting: " << socket->error() << "\n";
    }
    socket->close();
    cleanup();    
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();
    m_logoVbo.destroy();
    delete m_program;
    m_program = nullptr;
    doneCurrent();
    QObject::disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
}

void GLWidget::connected()
{
    qDebug() << "Connected in graphics widget";
}

void GLWidget::disconnected()
{
    qDebug() << "Disconnected in graphics widget";
}

void GLWidget::readyRead()
{
    QString X_idx;
    QString Y_idx;
    QString Z_idx;
    int X, Y, Z;
    QStringList str_list;

//    qDebug() << "Reading: " << socket->bytesAvailable();

    QString str( socket->readAll());
    if (str == "----\r\n")
    {
//        qDebug() << str;
        socket->write("S\r\n");
    }
    else
    {
        m_logo.clear_leds();
        str_list = str.split(":", Qt::SkipEmptyParts);
        while (!(str_list.size() < 3) && str_list.first() != "----\r\n")
        {
            X_idx = str_list.takeFirst();
            Y_idx = str_list.takeFirst();
            Z_idx = str_list.takeFirst();

//            qDebug() << X_idx<< ":" << Y_idx<< ":" << Z_idx << ":";
            X = m_logo.Cube_coords.value(X_idx);
            Y = m_logo.Cube_coords.value(Y_idx);
            Z = m_logo.Cube_coords.value(Z_idx);
            m_logo.led_data[X][Y][Z].active = 1;
//            qDebug() << X<< " " << Y<< " " << Z;
        }
    }
    update();
}

void GLWidget::bytesWritten(const qint64 &bytes)
{
//    qDebug() << "Written " << bytes << " bytes";
}

static const char *vertexShaderSourceCore =
    "#version 150\n"
    "in vec4 vertex;\n"
    "in vec3 normal;\n"
    "out vec3 vert;\n"
    "out vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 150\n"
    "in highp vec3 vert;\n"
    "in highp vec3 vertNormal;\n"
    "out highp vec4 fragColor;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"  //COLOR 1
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   fragColor = vec4(col, 1.0);\n"
    "}\n";

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "//varying highp vec3 vert;\n"
    "//varying highp vec3 vertNormal;\n"
    "//uniform highp vec3 lightPos;\n"
    "uniform highp vec3 u_color;\n"
    "void main() {\n"
    "   //highp vec3 L = normalize(lightPos - vert);\n"
    "   //highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   //highp vec3 color = vec3(0.35, 0.9, 1.0);\n" //Lights on
    "   //highp vec3 color = vec3(0.0, 0.0, 1.0);\n" //Lights off
    "   //highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   //gl_FragColor = vec4(col, 1.0);\n"
    "   gl_FragColor = vec4(u_color, 1.0);\n"
    "}\n";

void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");
    // Custom shader variables:
    m_colorLoc = m_program->uniformLocation("u_color");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_logoVbo.create();
    m_logoVbo.bind();
    m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    // Our camera never changes in this example.
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -1);

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));

    m_program->release();
}

void GLWidget::setupVertexAttribs()
{
    m_logoVbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),
                             nullptr);
    m_logoVbo.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_world.setToIdentity();
    m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    QVector3D Vec3D_LightOn(0.35, 0.9, 1.0);
    QVector3D Vec3D_LightOff(0.0, 0.0, 1.0);

    for (int i = 0; i < MAX_LEDS_X; ++i)
    {
        for (int j = 0; j < MAX_LEDS_Y; ++j)
        {
            for (int k = 0; k < MAX_LEDS_Z; ++k)
            {
                if (m_logo.led_data[i][j][k].active)
                    m_program->setUniformValue(m_colorLoc, Vec3D_LightOn);
                else
                    m_program->setUniformValue(m_colorLoc, Vec3D_LightOff);

                glDrawArrays(GL_TRIANGLES                               // Draw mode
                             ,m_logo.led_data[i][j][k].startingVertex   // Starting index
                             ,36                                        // Length
                            );
            }
        }
    }
m_program->release();
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->position().toPoint();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->position().toPoint().x() - m_lastPos.x();
    int dy = event->position().toPoint().y() - m_lastPos.y();

    if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->position().toPoint();
}
