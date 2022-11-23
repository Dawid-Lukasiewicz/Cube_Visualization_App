// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef LOGO_H
#define LOGO_H

#include <qopengl.h>
#include <QList>
#include <QMap>
#include <QVector3D>

#define MAX_LEDS_X 5
#define MAX_LEDS_Y 5
#define MAX_LEDS_Z 5
#define MAX_LED_AMOUNT 125

#define X0 15U
#define X1 13U
#define X2 12U
#define X3 11U
#define X4 10U
#define Y0 14U
#define Y1 16U
#define Y2 17U
#define Y3 18U
#define Y4 19U
#define Z0 27U
#define Z1 26U
#define Z2 22U
#define Z3 21U
#define Z4 20U

const unsigned int X_table[MAX_LEDS_X] = {X0, X1, X2, X3, X4};
const unsigned int Y_table[MAX_LEDS_Y] = {Y0, Y1, Y2, Y3, Y4};
const unsigned int Z_table[MAX_LEDS_Z] = {Z0, Z1, Z2, Z3, Z4};

struct Led
{
    int startingVertex;
    int active = 0;
};

class Logo
{
public:
    Logo();
    const GLfloat *constData() const { return m_data.constData(); }
    int count() const { return m_count; }
    int vertexCount() const { return m_count / 6; }
    void clear_leds();

    //Variables
    Led led_data[MAX_LEDS_X][MAX_LEDS_Y][MAX_LEDS_Z];
    QMap<QString, unsigned int> Cube_coords;

private:
    int activate_led(int X, int Y, int Z);
    void add(const QVector3D &v, const QVector3D &n);
    void create_led(GLfloat x, GLfloat y, GLfloat z, int x_idx, int y_idx, int z_idx);
    void cube(GLfloat x, GLfloat y, GLfloat z);
    void rectangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4, GLfloat z);
    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
    void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

    QList<GLfloat> m_data;
    int m_count = 0;
};

#endif // LOGO_H
