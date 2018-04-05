#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

int originX = 0;
int originY = 0;

bool linked = true;

int W = 800;
int H = 800;
double midX = 0;
double midY = 0;
double radiusX = 5;
double radiusY = 5;

int maxIterations = 2;
float scale = 0.2;

vector<float> pixels(3 * W * H);

int countIterations(double n, double x, double y, int maxIterations) {
    double r, i, rtemp, itemp;
    r = i = 0;
    int iterations = 0;
    while ((r * r) + (i * i) < 4 && iterations < maxIterations) {
        rtemp = (r * r) - (i * i) + x;
        itemp = (2 * r * i) + y;
        //rtemp = (r * r * r) - (3 * r * i * i) + x;
        //itemp = (3 * r * r * i) - (i * i * i) + y;
        //rtemp = pow((r * r) + (i * i), n / 2) * cos(n * atan2(i, r)) + x;
        //itemp = pow((x * x) + (y * y), n / 2) * sin(n * atan2(i, r)) + y;
        if (r == rtemp && i == itemp) {
            iterations = maxIterations;
            break;
        }
        r = rtemp;
        i = itemp;
        ++iterations;
    }
    return iterations;
}

float colors[5][3] = {
    {249,83,255},
    {44,255,243},
    {167,255,66},
    {255,198,39},
    {255,109,75}
};

void writePixel(vector<float>& pixels, int i, int iterations, int maxIterations) {
    if (iterations == maxIterations) {
        pixels[i] = 0;
        pixels[i + 1] = 0;
        pixels[i + 2] = 0;
    } else {
        //pixels[i + 0] = (float) (iterations) / maxIterations;
        //pixels[i + 1] = (float) (iterations) / maxIterations;
        //pixels[i + 2] = (float) (iterations) / maxIterations;
        //pixels[i + 0] = (float) (iterations % 2) / 2;
        //pixels[i + 1] = (float) (iterations % 3) / 3;
        //pixels[i + 2] = (float) (iterations % 5) / 5;
        pixels[i + 0] = colors[iterations % 5][0] / 255;
        pixels[i + 1] = colors[iterations % 5][1] / 255;
        pixels[i + 2] = colors[iterations % 5][2] / 255;
    }
}

void writePixels(vector<float>& pixels, int W, int H, double minX, double minY, double pixelWidth, double pixelHeight, int maxIterations) {
    for (int X = 0; X < W; ++X) {
        for (int Y = 0; Y < H; ++Y) {
            writePixel(pixels, 3 * ((Y * W) + X), countIterations(2, (pixelWidth * X) + minX, (pixelHeight * Y) + minY, maxIterations), maxIterations);
        }
    }
}

void renderBitmapString(float x, float y, void *font, char *string) {
    char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
    glRasterPos2i(-1, -1);
}

void renderScene(void) {
    writePixels(pixels, W, H, midX - radiusX, midY - radiusY, 2 * radiusX / W, 2 * radiusY / H, maxIterations);
    glDrawPixels(W, H, GL_RGB, GL_FLOAT, &pixels.front());
    
    renderBitmapString(-0.9, -0.95, GLUT_BITMAP_9_BY_15, "Copyright 2018 Dead Horse Labs");
    char s[50];
    sprintf(s, "Center X: %f", midX);
    renderBitmapString(-0.9, 0.9, GLUT_BITMAP_9_BY_15, s);
    sprintf(s, "Center Y: %f", midY);
    renderBitmapString(-0.9, 0.85, GLUT_BITMAP_9_BY_15, s);
    sprintf(s, "Radius: %f", min(radiusX, radiusY));
    renderBitmapString(-0.9, 0.8, GLUT_BITMAP_9_BY_15, s);
    sprintf(s, "Iterations: %i", maxIterations);
    renderBitmapString(-0.9, 0.75, GLUT_BITMAP_9_BY_15, s);
    sprintf(s, "Scaling factor: %0.2f%%", 100 * scale);
    renderBitmapString(-0.9, 0.7, GLUT_BITMAP_9_BY_15, s);
    if (linked) {
        renderBitmapString(-0.9, 0.65, GLUT_BITMAP_9_BY_15, "Press (L) to unLink zoom and iterations.");
    } else {
        renderBitmapString(-0.9, 0.65, GLUT_BITMAP_9_BY_15, "Press (L) to Link zoom and iterations.");
    }
    renderBitmapString(-0.9, 0.6, GLUT_BITMAP_9_BY_15, "Press (H) to go Home.");
    renderBitmapString(-0.9, 0.55, GLUT_BITMAP_9_BY_15, "Press (S) to Save the current image.");
    renderBitmapString(-0.9, 0.5, GLUT_BITMAP_9_BY_15, "Press (Esc) to quit.");
    
    glutSwapBuffers();
}

void changeSize(int newW, int newH) {
    radiusX *= (double) (newW) / W;
    radiusY *= (double) (newH) / H;
    W = newW;
    H = newH;
    pixels.resize(3 * W * H);
}

// -----------------------------------
//             KEYBOARD
// -----------------------------------

void processNormalKeys(unsigned char key, int X, int Y) {
    switch (key) {
        case 9:
            scale += .01;
            if (scale > 0.5) {
                scale = 0.01;
            }
            break;
        case 27:
            exit(0);
            break;
        case 104:
            linked = true;
            midX = 0;
            midY = 0;
            radiusX = 5;
            radiusY = H * 5 / (double) (W);
            maxIterations = 2;
            scale = .2;
            glutWarpPointer(W / 2, H / 2);
            break;
        case 108:
            linked ^= true;
            break;
        case 115:
            string filename = to_string(midX) + "," + to_string(midY) + "/" + to_string(min(radiusX, radiusY)) + ".ppm";
            ofstream file(filename.c_str());
            
            if (file.is_open()) {
                file << "P3" << endl;
                file << W << " " << H << endl;
                file << 255 << endl;
                
                for (int Y = H; Y > 0; --Y) {
                    for (int X = 0; X < W; ++X) {
                        for (int Z = 0; Z < 3; ++Z) {
                            file << (int) (256 * pixels[3 * ((Y * W) + X) + Z]) << " ";
                        }
                        file << " ";
                    }
                    file << endl;
                }
                
                file.close();
            }
    }
    glutPostRedisplay();
}

void pressKey(int key, int X, int Y) {
    switch (key) {
        case GLUT_KEY_UP:
            if (linked) {
                ++maxIterations;
            }
            if (maxIterations > 1) {
                radiusX *= (1 - scale);
                radiusY *= (1 - scale);
                midX += 2 * scale * radiusX * (((double) (X) / W) - 0.5);
                midY -= 2 * scale * radiusY * (((double) (Y) / H) - 0.5);
            }
            break;
        case GLUT_KEY_DOWN:
            if (maxIterations > 1) {
                radiusX *= (1 / (1 - scale));
                radiusY *= (1 / (1 - scale));
                midX -= 2 * scale * radiusX * (((double) (X) / W) - 0.5);
                midY += 2 * scale * radiusY * (((double) (Y) / H) - 0.5);
                if (linked) {
                    --maxIterations;
                }
            }
            break;
        case GLUT_KEY_LEFT:
            if (maxIterations > 1) {
                --maxIterations;
                if (linked) {
                    radiusX *= (1 / (1 - scale));
                    radiusY *= (1 / (1 - scale));
                    midX -= 2 * scale * radiusX * (((double) (X) / W) - 0.5);
                    midY += 2 * scale * radiusY * (((double) (Y) / H) - 0.5);
                }
            }
            break;
        case GLUT_KEY_RIGHT:
            ++maxIterations;
            if (linked) {
                radiusX *= (1 - scale);
                radiusY *= (1 - scale);
                midX += 2 * scale * radiusX * (((double) (X) / W) - 0.5);
                midY -= 2 * scale * radiusY * (((double) (Y) / H) - 0.5);
            }
            break;
    }
    glutPostRedisplay();
}

// -----------------------------------
//             MOUSE
// -----------------------------------
void mouseMove(int X, int Y) {
    if (originX && originY) {
        midX -= 2 * radiusX * (((double) (X - originX) / W));
        midY += 2 * radiusY * (((double) (Y - originY) / H));
        originX = X;
        originY = Y;
        glutPostRedisplay();
    }
}

void mouseButton(int button, int state, int X, int Y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            originX = X;
            originY = Y;
        } else {
            originX = 0;
            originY = 0;
        }
    }
}

// -----------------------------------
//             MAIN and INIT
// -----------------------------------

void init() {
    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    //glutIdleFunc(renderScene);
    
    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(pressKey);
    //glutSpecialUpFunc(releaseKey);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);
    
    glutWarpPointer(W / 2, H / 2);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(W, H);
    //glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Interactive Fractal Explorer");
    
    init();
    
    glutMainLoop();
    
    return 0;
}
