#include "glwindow.h"
#include <QColorDialog>
#include <QDebug>
#include "model.h"
#include <GL/glu.h>
#include "vector3.h"
#include <QApplication>
#include "math.h"

GLWindow::GLWindow(QWidget* parent) : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
     setFocusPolicy(Qt::StrongFocus);
}
void GLWindow::initializeGL()
{
    qglClearColor(Qt::black);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
  //  installEventFilter(this);
}

void GLWindow::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat x = (GLfloat)width / height;
    glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);

    updateWindow();
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double aspect = width() / height();
    glViewport(0, 0, width(), height());
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-150, 150, -112, 112, -150, 1500);//glOrtho(hor+, hor-, up+ , up-, nearPlane, farPlane);
    gluPerspective(60.0, aspect, 0.01, 2000.0);//fov,aspect
    glMatrixMode(GL_MODELVIEW);

    for(int i = 0; i < models.size();i++){
        models[i]->draw();
    }
}

void GLWindow::mousePressEvent(QMouseEvent *event){
    lastPos = event->pos();
}

void GLWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    int face = faceAtPosition(event->pos());
    if (face != -1) {
        QColor color = QColorDialog::getColor( curmodel->faceColors[face],
                                               this);
        if (color.isValid()) {
            curmodel->faceColors[face] = color;
        }
    }
}
bool GLWindow::eventFilter(QObject* obj, QEvent* event){
    if(event->type() == QEvent::MouseMove){
        if(!isGameMode)
            return false;
        static bool settedPos;
        if((settedPos = !settedPos) == true){ // last move is correctly
            lastPos = QPoint(0,0);
            return false;
        }
        QMouseEvent* me = static_cast<QMouseEvent*>(event);

        float dx = sensivity * ((float) (me->x() - lastPos.x()) / width());/*
                                                        GLfloat dy = (GLfloat) (event->y() - lastPos.y()) / height();
                                                        Vector3 newRot = curmodel->getRotation();
                                                        if (event->buttons() & Qt::LeftButton) {
                                                            newRot.x += 180 * dy;
                                                            newRot.y += 180 * dx;
                                                        } else if (event->buttons() & Qt::RightButton) {
                                                            newRot.x += 180 * dy;
                                                            newRot.z += 180 * dx;
                                                        }
                                                        curmodel->setRotation(newRot);*/
        int posW = cursor().pos().x();
        int posH = cursor().pos().y();

        if(posW > globPosX && posW < globWidth &&// intersect cursor
                posH > globPosY && posH < globHeight){


            float differenceW = posW - (globWidth + globPosX) / 2;
            float differenceH = posH - (globHeight + globPosY) / 2;
            qDebug() << dx;

            if(differenceW > 0)
                rotateCamera(dx);
            if(differenceW < 0)
                rotateCamera(-dx);


            lastPos = me->pos();
            cursor().setPos((globWidth + globPosX) / 2, (globHeight + globPosY) / 2);
        }
    }
    else if(event->type() == QEvent::Leave){
        if(isGameMode){
            cursor().setPos((globWidth + globPosX) / 2, (globHeight + globPosY) / 2);
        }
    }
    return false;
}
void GLWindow::keyPressEvent(QKeyEvent* event){
    int key = event->key();//event->key() - целочисленный код клавиши
    QString inputChar = (QString(QChar(key)));
    qDebug() << inputChar;
    if(event->key() == Qt::Key_Escape){
        if(isGameMode){
            slotChangeGameMode();
        }
    }
}
int GLWindow::faceAtPosition(const QPoint &pos)
{
    const int MaxSize = 512;
    GLuint buffer[MaxSize];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix((GLdouble)pos.x(),
                  (GLdouble) (viewport[3] - pos.y()),
            5.0, 5.0, viewport);

    GLfloat x = (GLfloat)width() / height();
    glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
    for(int i = 0; i < models.size();i++){
        models[i]->draw();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    if (!glRenderMode(GL_RENDER))
        return -1;
    return buffer[3];
}

void GLWindow::update(){  
    updateGL();
}
void GLWindow::rotateCamera(float multiply){
    yCamRot += multiply;
    for(int i = 0; i< models.size();i++){
        Vector3 mRot = models[i]->getRotation();
        models[i]->setRotation(Vector3(mRot.x, mRot.y + multiply, mRot.z));
    }
    emit signalChangeYCamRot(yCamRot);    
}
void GLWindow::updateWindow(){
    globPosX = mapToGlobal(pos()).x();
    globPosY = mapToGlobal(pos()).y();
    globWidth = mapToGlobal(QPoint(width(), height())).x();
    globHeight = mapToGlobal(QPoint(width(), height())).y();
}
void GLWindow::slotForwardMove(){
    move(forward, true);
}
void GLWindow::slotBackwardMove(){
    move(backward, false);
}
void GLWindow::slotRightMove(){
    move(right, false);
}
void GLWindow::slotLeftMove(){
    move(left, true);
}
void GLWindow::move(motionVector mVector,bool multiply){
    for(int i = 0; i< models.size();i++){
        Vector3 mPos = models[i]->getPosition();
        if(mVector == forward || mVector == backward){
            mPos.z +=  cos(yCamRot * M_PI / 180) * speed * (multiply ? 1 : -1);
            mPos.x +=  sin(yCamRot * M_PI / 180) * -speed * (multiply ? 1 : -1);
        }
        else{
            mPos.z +=  sin(yCamRot * M_PI / 180) * speed * (multiply ? 1 : -1);
            mPos.x +=  cos(yCamRot * M_PI / 180) * -speed * (multiply ? -1 : 1);
        }
        models[i]->setPosition(mPos);
    }
    qDebug() <<  "z is " +  QString::number(cos(yCamRot * M_PI / 180) * speed);
    qDebug() << "x is " +  QString::number(sin(yCamRot * M_PI / 180) * speed);
    qDebug() <<mVector << multiply;
}
void GLWindow::slotChangeGameMode(){
    isGameMode = !isGameMode;
    qDebug()<<isGameMode;

    setMouseTracking(isGameMode);
    if(isGameMode){
        ShowCursor(FALSE);
    }
    else{
        ShowCursor(TRUE);
    }
}
