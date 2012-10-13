#include "dmcolorramp.h"
#include <QColor>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#endif
#include <glext.h>

namespace DM {

GLuint hue(QColor start, QColor stop) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    
    GLubyte data[256][4];
    
    double h_inc = (stop.hueF() - start.hueF()) / 256.0;
    double s = stop.saturationF();
    double v = stop.valueF();
    
    
    for (int i = 0; i < 256; i++) {
        double h = start.hueF() + (i*h_inc);

        QColor c = QColor::fromHsvF(h, s, v);
        data[i][0] = c.red();
        data[i][1] = c.green();
        data[i][2] = c.blue();
        data[i][3] = c.alpha();
    }
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    Q_ASSERT(glIsTexture(texture));
    return texture;
}

GLuint val(QColor start, QColor stop) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    
    GLubyte data[256][4];
    
    double h = stop.hueF();
    double s = stop.saturationF();
    double v_inc = (stop.valueF() - start.valueF()) / 256.0;
    
    for (int i = 0; i < 256; i++) {
        double v = start.valueF() + (i*v_inc);

        QColor c = QColor::fromHsvF(h, s, v);
        data[i][0] = c.red();
        data[i][1] = c.green();
        data[i][2] = c.blue();
        data[i][3] = c.alpha();
    }
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    Q_ASSERT(glIsTexture(texture));
    return texture;
}

GLuint sat(QColor start, QColor stop) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    
    GLubyte data[256][4];
    
    double h = stop.hueF();
    double s_inc = (stop.saturationF() - start.saturationF()) / 256.0;
    double v = stop.valueF();
    
    for (int i = 0; i < 256; i++) {
        double s = start.saturationF() + (i*s_inc);

        QColor c = QColor::fromHsvF(h, s, v);
        data[i][0] = c.red();
        data[i][1] = c.green();
        data[i][2] = c.blue();
        data[i][3] = c.alpha();
    }
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    Q_ASSERT(glIsTexture(texture));
    return texture;
}

GLuint hsv(QColor start, QColor stop) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    
    GLubyte data[256][4];
    
    double h_inc = (stop.hueF() - start.hueF()) / 256.0;
    double s_inc = (stop.saturationF() - start.saturationF()) / 256.0;
    double v_inc = (stop.valueF() - start.valueF()) / 256.0;
    
    for (int i = 0; i < 256; i++) {
        double h = start.hueF() + (i*h_inc);
        double s = start.saturationF() + (i*s_inc);
        double v = start.valueF() + (i*v_inc);

        QColor c = QColor::fromHsvF(h, s, v);
        data[i][0] = c.red();
        data[i][1] = c.green();
        data[i][2] = c.blue();
        data[i][3] = c.alpha();
    }
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    Q_ASSERT(glIsTexture(texture));
    return texture;
}

GLuint black_to_red() {
    return val(Qt::black, Qt::red);
}

GLuint black_to_blue() {
    return val(Qt::black, Qt::blue);
}

GLuint black_to_green() {
    return val(Qt::black, Qt::green);
}

GLuint black_to_yellow() {
    return val(Qt::black, Qt::yellow);
}

GLuint black_to_orange() {
    return val(Qt::black, QColor(255, 165, 0));
}

GLuint white_to_red() {
    return sat(Qt::white, Qt::red);
}

GLuint white_to_blue() {
    return sat(Qt::white, Qt::blue);
}

GLuint white_to_green() {
    return sat(Qt::white, Qt::green);
}

GLuint white_to_yellow() {
    return sat(Qt::white, Qt::yellow);
}

GLuint white_to_orange() {
    return sat(Qt::white, QColor(255, 165, 0));
}

GLuint white_to_black() {
    return val(Qt::white, Qt::black);
}

GLuint rainbow() {
    QColor start, stop;
    start.setHsv(359, 255, 255);
    stop.setHsv(58, 255, 255);
    return hue(start, stop);
}

GLuint light_green_to_dark_green() {
    QColor start, stop;
    start.setHsv(149, 128, 255);
    stop.setHsv(149, 128, 70);
    return val(start, stop);
}

GLuint light_blue_to_dark_blue() {
    QColor start, stop;
    start.setHsv(240, 70, 255);
    stop.setHsv(240, 255, 255);
    return sat(start, stop);
}

GLuint yellow_to_brown() {
    QColor start, stop;
    start.setHsv(29, 177, 127);
    stop.setHsv(60, 255, 255);
    return hsv(start, stop);
}

typedef GLuint (*color_ramp_func)(void);

GLuint get_color_ramp(ColorRamp ramp) {
    color_ramp_func funcs[] = {
        black_to_red,
        white_to_red,
        black_to_green,
        white_to_green,
        black_to_blue,
        white_to_blue,
        
        black_to_yellow,
        white_to_yellow,
        black_to_orange,
        white_to_orange,
        
        white_to_black,
        rainbow,
        light_green_to_dark_green,
        light_blue_to_dark_blue,
        yellow_to_brown
    };
    return funcs[ramp]();
}

} // namespace DM
