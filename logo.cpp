// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "logo.h"
#include <qmath.h>

Logo::Logo()
{
    m_data.resize(5500 * 6);

    const GLfloat x1 = +0.14f;
    const GLfloat y1 = +0.14f;

    const GLfloat x2 = +0.14f;
    const GLfloat y2 = -0.14f;

    const GLfloat x3 = -0.14f;
    const GLfloat y3 = +0.14f;

    const GLfloat x4 = -0.14f;
    const GLfloat y4 = -0.14f;

    cube(0.1f, -0.1f, 0.1f);
}


void Logo::add(const QVector3D &v, const QVector3D &n)
{
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    m_count += 6;
}

void Logo::led(GLfloat x, GLfloat y, GLfloat z)
{
    const GLfloat offset = 0.03f;
    QVector3D n = QVector3D::normal(
                QVector3D(x, y, z)
                ,QVector3D(x+offset, y+offset, z));

    /* First wall */
//    triangle
    add(QVector3D(x, y, z), n);
    add(QVector3D(x+offset, y, z), n);
    add(QVector3D(x+offset, y+offset, z), n);

//    triangle
    add(QVector3D(x, y, z), n);
    add(QVector3D(x+offset, y+offset, z), n);
    add(QVector3D(x, y+offset, z), n);

    /* Second wall */
    add(QVector3D(x+offset, y+offset, z), n);
    add(QVector3D(x+offset, y, z), n);
    add(QVector3D(x+offset, y+offset, z+offset), n);

    add(QVector3D(x+offset, y+offset, z+offset), n);
    add(QVector3D(x+offset, y, z), n);
    add(QVector3D(x+offset, y, z+offset), n);

    /* Third wall */
    add(QVector3D(x+offset, y+offset, z+offset), n);
    add(QVector3D(x+offset, y, z+offset), n);
    add(QVector3D(x, y+offset, z+offset), n);

    add(QVector3D(x, y, z+offset), n);
    add(QVector3D(x, y+offset, z+offset), n);
    add(QVector3D(x+offset, y, z+offset), n);

    /* Fourth wall */
    add(QVector3D(x, y, z), n);
    add(QVector3D(x+offset, y, z+offset), n);
    add(QVector3D(x+offset, y, z), n);

    add(QVector3D(x, y, z), n);
    add(QVector3D(x, y, z+offset), n);
    add(QVector3D(x+offset, y, z+offset), n);

    /* Fifth wall */
    add(QVector3D(x, y, z), n);
    add(QVector3D(x, y+offset, z), n);
    add(QVector3D(x, y, z+offset), n);

    add(QVector3D(x, y+offset, z), n);
    add(QVector3D(x, y+offset, z+offset), n);
    add(QVector3D(x, y, z+offset), n);

    /* Sixth wall */
    add(QVector3D(x, y+offset, z), n);
    add(QVector3D(x+offset, y+offset, z), n);
    add(QVector3D(x+offset, y+offset, z+offset), n);

    add(QVector3D(x, y+offset, z+offset), n);
    add(QVector3D(x, y+offset, z), n);
    add(QVector3D(x+offset, y+offset, z+offset), n);
}

void Logo::cube(GLfloat x, GLfloat y, GLfloat z)
{
    const GLfloat offset = 0.15f;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            for (int k = 0; k < 5; ++k)
            {
                led(x+k*offset, y+i*offset, z+j*offset);
            }
        }
    }
}

void Logo::rectangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4, GLfloat z)
{
    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, z), QVector3D(x2 - x1, y2 - y1, z));
    add(QVector3D(x1, y1, z), -n);
    add(QVector3D(x2, y2, z), -n);
    add(QVector3D(x3, y3, z), -n);
    add(QVector3D(x4, y4, z), -n);
}

void Logo::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);

    add(QVector3D(x3, y3, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);

    n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    add(QVector3D(x4, y4, 0.05f), n);
    add(QVector3D(x1, y1, 0.05f), n);
    add(QVector3D(x2, y2, 0.05f), n);

    add(QVector3D(x2, y2, 0.05f), n);
    add(QVector3D(x3, y3, 0.05f), n);
    add(QVector3D(x4, y4, 0.05f), n);
}

void Logo::extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -0.1f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);

    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);
}
