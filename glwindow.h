#ifndef GLWINDOW_H
#define GLWINDOW_H
#include <QGLWidget>
#include <QMouseEvent>
#include "component.h"
#include "vector3.h"

class Model;

class GLWindow: public QGLWidget, public Component
{
    Q_OBJECT
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();    
    void mouseDoubleClickEvent(QMouseEvent *event);
    bool eventFilter(QObject* obj, QEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
public:    
    Model* curmodel;
    GLWindow(QWidget* parent = 0);
    QList<Model*> models;
    Vector3 camRotation;
    void update();   
private:
    enum axis {horizontal, vertical};
    void rotateCamera(axis ax,float multiply);
    int faceAtPosition(const QPoint &pos);    
    int globPosX;
    int globPosY;
    int globWidth;
    int globHeight;
    float speed = 10;
    float sensivity = 3;
    bool isGameMode = false;

    bool keyPress_W = false;
    bool keyPress_A = false;
    bool keyPress_S = false;
    bool keyPress_D = false;

    void updateWindow();
    enum motionVector{forward, backward, right, left};
    void move(motionVector mVector,bool multiply);
public slots:    
    void slotChangeGameMode();
signals:
    void signalChangeCamRot(Vector3 vec);

};

#endif // GLWINDOW_H
