#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <SOIL/SOIL.h>

int i = 0;
void displayMe(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POLYGON);
    i = i % 3;
    glVertex3f(0, 0.0, 0.5);
    glVertex3f(i, 0.0, 0.0);
    glVertex3f(0.0, 0.5, 0.0);
    glVertex3f(0.0, 0.0, 0.5);
    i++;

    glEnd();
    glFlush();

    int width, height;
    unsigned char *image = SOIL_load_image("pics/test.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(400, 300);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello world!");
    glutDisplayFunc(displayMe);
    glutMainLoop();
    return 0;
}