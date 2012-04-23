/*
  stlviewer.cpp

  Copyright (c) 2012, Jeremiah LaRocco jeremiah.larocco@gmail.com

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <QMainWindow>

#include <sstream>
#include <stdexcept>

#include "stlviewer.h"

/*!
  Initializes the object and sets the OpenGL format.
*/
STLViewer::STLViewer(QWidget*) : stlf(new STLFile()), rotationX(0.0), rotationY(0.0),
                                 rotationZ(0.0), translate(250.0),
                                 num_tris(0), verts(0), norms(0), indices(0)
 {
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
}

/*!
  Frees memory and cleans up OpenGL state
*/
STLViewer::~STLViewer() {
    for (size_t i=1;i<NUM_LISTS; ++i) {
        glDeleteLists(dispLists[i], 1);
    }
    if (verts) {
        delete [] verts;
        verts = 0;
    }
    if (norms) {
        delete [] norms;
        norms = 0;
    }
    if (indices) {
        delete [] indices;
        indices = 0;
    }
    if (stlf) {
        delete stlf;
        stlf = 0;
    }
}

/*!
  Loads the material arrays
*/
void STLViewer::initMaterials() {
    // lines
    mat_specular[LINE_MAT][0]=0.0f;
    mat_specular[LINE_MAT][1]=0.0f;
    mat_specular[LINE_MAT][2]=0.0f;
    mat_specular[LINE_MAT][3]=1.0f;
  
    mat_shininess[LINE_MAT][0]=80.0f;

    mat_diffuse[LINE_MAT][0]=0.0f;
    mat_diffuse[LINE_MAT][1]=0.0f;
    mat_diffuse[LINE_MAT][2]=0.0f;
    mat_diffuse[LINE_MAT][3]=1.0f;
  
    mat_ambient[LINE_MAT][0] = 0.0f;
    mat_ambient[LINE_MAT][1] = 0.0f;
    mat_ambient[LINE_MAT][2] = 0.0f;
    mat_ambient[LINE_MAT][3] = 1.0f;

    // closed spots
    mat_specular[SURF_MAT][0]=1.0f;
    mat_specular[SURF_MAT][1]=1.0f;
    mat_specular[SURF_MAT][2]=1.0f;
    mat_specular[SURF_MAT][3]=1.0f;

    mat_shininess[SURF_MAT][0]=100.0f;
  
    mat_diffuse[SURF_MAT][0]=0.0f;
    mat_diffuse[SURF_MAT][1]=0.0f;
    mat_diffuse[SURF_MAT][2]=1.0f;
    mat_diffuse[SURF_MAT][3]=1.0f;
  
    mat_ambient[SURF_MAT][0] = 0.10f;
    mat_ambient[SURF_MAT][1] = 0.10f;
    mat_ambient[SURF_MAT][2] = 0.10f;
    mat_ambient[SURF_MAT][3] = 1.0f;
}

/*!
  Initializes the light arrays
*/
void STLViewer::initLights() {
    light_position[0][0]=0.0f;
    light_position[0][1]=0.0f;
    light_position[0][2]=30.0f;
    light_position[0][3]=1.0f;
  
    light_position[1][0]=0.0f;
    light_position[1][1]=0.0f;
    light_position[1][2]=-30.0f;
    light_position[1][3]=1.0f;
  
    for (size_t i=0;i<NUM_LIGHTS; ++i) {
        light_color[i][0]=1.0f;
        light_color[i][1]=1.0f;
        light_color[i][2]=1.0f;
        light_color[i][3]=1.0f;
        lmodel_ambient[i][0]=0.4f;
        lmodel_ambient[i][1]=0.4f;
        lmodel_ambient[i][2]=0.4f;
        lmodel_ambient[i][3]=1.0f;
    }

    // glEnable(GL_LIGHTING);
    
    // glEnable(GL_LIGHT0);
    // glEnable(GL_LIGHT1);
}

/*!
  Initializes the display lists
*/
void STLViewer::initLists() {
    dispLists[0] = glGenLists(1);
    // glNewList(dispLists[0], GL_COMPILE);
    // drawBoxList(1);
    // glEndList();
}

void STLViewer::regenList() {
    glDeleteLists(dispLists[0], 1);
    dispLists[0] = glGenLists(1);
    glNewList(dispLists[0], GL_COMPILE);
    if (stlf) {
        if (verts) {
            delete [] verts;
            verts = 0;
        }
        if (norms) {
            delete [] norms;
            norms = 0;
        }
        if (indices) {
            delete [] indices;
            indices = 0;
        }

        num_tris = stlf->getNumTris();
        qDebug() << "Num tris: " << num_tris;
        verts = new float[num_tris*3*3];
        norms = new float[num_tris*3*3];
        indices = new unsigned int[num_tris*3];

        stlf->fillBuffers(num_tris, verts, norms, indices);

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[SURF_MAT]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[SURF_MAT]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[SURF_MAT]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[SURF_MAT]);

        glEnableClientState( GL_VERTEX_ARRAY);
        // glEnableClientState(GL_NORMAL_ARRAY);

        glVertexPointer( 3, GL_FLOAT, 0, verts );
        // glNormalPointer( GL_FLOAT, 0, norms );
        glColor3f(1.0,0.0,0.0);
        glDrawElements( GL_TRIANGLES, 3*num_tris, GL_UNSIGNED_INT, indices);

        glColor3f(0.0,0.0,0.0);
        for (size_t i=0;i<num_tris;++i) {
            glDrawElements( GL_LINE_LOOP, 3, GL_UNSIGNED_INT, indices+3*i);
        }

        // glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    glEndList();

}
/*!
  Initializes OpenGL by enabling required features and loading materials/lights/display lists
*/
void STLViewer::initializeGL() {
    // Enable stuff
    qglClearColor(Qt::white);
  
    glShadeModel(GL_SMOOTH);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_LINE_SMOOTH);
    
    glEnable(GL_BLEND);
    
    //   glBlendFunc(GL_ONE, GL_ZERO);
  
    // Load materials/lights/textures
    initMaterials();
    initLights();

    // Generate display lists
    initLists();
}

/*!
  Called automatically when the window is rezied
*/
void STLViewer::resizeGL(int width, int height) {
    glViewport(0,0, (GLsizei) width, (GLsizei)height);
  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, 1.0, 1.0, 1000);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*!
  Determines the unique name of the thing at pos
*/
int STLViewer::nameAtPos(const QPoint &pos) {

    const int MaxSize = 2048;
    GLuint buffer[MaxSize];
    GLint viewport[4];

    // Picking requires drawing the scene in GL_SELECT mode,
    // which requires a little setup
  
    // For details on how this work, see the "OpenGL Programming Guide"
    // All editions should give the details, but the fifth edition,
    // pages 570-580 is where I got it.

    // The general idea is to draw a tiny portion of the screen around
    // the mouse position in GL_SELECT mode, then switch back to GL_RENDER
    // mode, which will return the number of hits and populate "buffer".
    // Then search for the closest hit.

    glGetIntegerv(GL_VIEWPORT, viewport);

    glSelectBuffer(MaxSize, buffer);
  
    glRenderMode(GL_SELECT);
  
    glInitNames();
    glPushName(0);

    // setup the viewing transformation
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Make it use a 2.0x2.0 area round the mouse click
    gluPickMatrix(GLdouble(pos.x()), GLdouble(viewport[3]-pos.y()),
                  2.0,2.0, viewport);
    // The regular view transformation
    gluPerspective(80, 1.0, 1.0, 180);

    // Switch to GL_MODELVIEW and draw the scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  
    paintGL();

    // Reset the view and model transforms
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Switch back to GL_RENDER and get the number of hits
    int numHits = glRenderMode(GL_RENDER);

    // Return -1 if no hits
    if (numHits <=0)
        return -1;

    int closest = 0;

    // Loop through the returned hits to determine the closest one
    for (int i=0;i<numHits;++i) {
        if (buffer[4*i+1]<buffer[4*closest+1] ||
            buffer[4*i+2]<buffer[4*closest+2]) {
            closest = i;
        }
    }

    // Return the closest hit
    return buffer[4*closest + 3];
}

/*!
  Called by the system to draw the display
*/
void STLViewer::paintGL() {

    // Rotate/translate the projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    
    glTranslatef(0.0,0.0,-(translate+5));
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationY, 0.0, 1.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);

    // Switch to modelview mode and draw the scene
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
  

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup the lights
    glLightfv(GL_LIGHT0, GL_POSITION, light_position[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color[0]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color[0]);
  
    glLightfv(GL_LIGHT1, GL_POSITION, light_position[1]);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color[1]);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_color[1]);
  
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient[0]);
    glLoadIdentity();

    if (stlf) {
        glLoadName(1);
        // glEnableClientState( GL_VERTEX_ARRAY);
        // glEnableClientState(GL_NORMAL_ARRAY);
        // // glEnableClientState( GL_INDEX_ARRAY );
 
        // // glIndexPointer( GL_UNSIGNED_INT, 0, indices );
        // glVertexPointer( 3, GL_FLOAT, 0, verts );
        // glNormalPointer( GL_FLOAT, 0, norms );
        // // qDebug() << "Outputing triangle " << num_tris;
        // // for (size_t i=0;i<num_tris;++i) {
        // //     qDebug() << indices[3*i+0] << indices[3*i+1] << indices[3*i+2];
        // // }
        // glDrawElements( GL_TRIANGLES, num_tris, GL_UNSIGNED_INT, indices);

        // // glDisableClientState(GL_INDEX_ARRAY);
        // glDisableClientState(GL_NORMAL_ARRAY);
        // glDisableClientState(GL_VERTEX_ARRAY);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[SURF_MAT]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[SURF_MAT]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[SURF_MAT]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[SURF_MAT]);

        glCallList(dispLists[0]);
    }

    // Reset to how we found things
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glFlush();
}

/*!
  Handles mouse clicks
*/
void STLViewer::mousePressEvent(QMouseEvent *event) {
    if (!stlf) {
        return;
    }

    clicked = true;

    // Used for picking, which isn't used
    // Try to get the name at this position
    // int temp = nameAtPos(event->pos());
  
    // Set the last postion for rotations
    lastPos = event->pos();
  
  
    // Update the display
    updateGL();
}

/*!
  Handles mouse moves
*/
void STLViewer::mouseMoveEvent(QMouseEvent *event) {

  
    GLfloat dx = GLfloat(event->x() - lastPos.x())/width();
    GLfloat dy = GLfloat(event->y() - lastPos.y())/height();

    // Rotate depending on which mouse button is clicked
    if (event->buttons() & Qt::LeftButton) {
        rotationX += 180*dy;
        rotationY += 180*dx;
        updateGL();
    } else if (event->buttons() & Qt::RightButton) {
        rotationX += 180*dy;
        rotationZ += 180*dx;
        updateGL();
    }
  
    // Save the current position
    lastPos = event->pos();
}

/*!
  Handle zooming
*/
void STLViewer::wheelEvent(QWheelEvent *event) {
    translate += event->delta()*(-0.125*0.5*0.5);
  
    if (translate<0.1) translate = 0.1;
    updateGL();
}

/*!
  Reset the view to the original setting.
*/
void STLViewer::resetView() {
    translate=10;
    if (stlf) {
        translate = 1.5*stlf->getBoundingRadius();
    }
    rotationX = 27.2457f;
    rotationY = -46.44f;
    rotationZ = 0.0f;
    updateGL();
}

/*!
  Debug function that will display information about OpenGL errors.
  The argument is the line number of the error
*/
inline void STLViewer::handleGLError(size_t ln) {
    GLenum ec = glGetError();
    // Do nothing if there's no error
    if (ec== GL_NO_ERROR) return;
  
    // Get the error information in a string
    std::ostringstream err;
    err << (const char*)gluErrorString(ec) << " : " << ln;

    // Print info to stdout and throw an exception
    throw new std::runtime_error(err.str());
}

bool STLViewer::openFile(QString fileName) {
    STLFile *newf;
    try {
        newf = new STLFile(fileName.toStdString());
    } catch (std::runtime_error re) {
        qDebug() << "Caught an exception";
        QMessageBox::critical(this, tr("STL Viewer"),
                              QString(re.what()));
        return false;
    }
    
    if (newf) {
        qDebug() << "Got new stl file, replacing old one.";
        if (stlf) {
            delete stlf;
        }
        stlf = newf;
        qDebug() << "Regenerating display list!";
        regenList();
        resetView();
        return true;
    } else {
        qDebug() << "newf was NULL";
    }
    return false;
}
