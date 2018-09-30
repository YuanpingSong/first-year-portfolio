//OpenMP version.  Edit and submit only this file.
/* Enter your details below
 * Name : Yuanping Song
 * UCLA ID:
 * Email id: ysong21@g.ucla.edu
 * Input Files: Old Files
 */
 
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int OMP_xMax;
#define xMax OMP_xMax
int OMP_yMax;
#define yMax OMP_yMax
int OMP_zMax;
#define zMax OMP_zMax
int OMP_zOffSet; // new
#define zOffSet OMP_zOffSet
int OMP_volume;
#define volume OMP_volume


int OMP_Index(int x, int y, int z)
{
    return ((z * yMax + y) * xMax + x);
}
#define Index(x, y, z) OMP_Index(x, y, z)

double OMP_SQR(double x)
{
    return x * x;
}
#define SQR(x) OMP_SQR(x)

double* OMP_conv;
double* OMP_g;

void OMP_Initialize(int xM, int yM, int zM)
{
    xMax = xM;
    yMax = yM;
    zMax = zM;
    zOffSet = xMax * yMax; // new
    assert(OMP_conv = (double*)malloc(sizeof(double) * xMax * yMax * zMax));
    assert(OMP_g = (double*)malloc(sizeof(double) * xMax * yMax * zMax));
}
void OMP_Finish()
{
    free(OMP_conv);
    free(OMP_g);
}
void OMP_GaussianBlur(double *u, double Ksigma, int stepCount)
{
    double lambda = (Ksigma * Ksigma) / (double)(2 * stepCount);
    double nu = (1.0 + 2.0*lambda - sqrt(1.0 + 4.0*lambda))/(2.0*lambda);
    int x, y, z, step, idx, offSetIdx;
    double boundryScale = 1.0 / (1.0 - nu);
    double postScale = pow(nu / lambda, (double)(3 * stepCount));
    int numThreads = omp_get_num_procs();
    
    for(step = 0; step < stepCount; step++)
    {
        omp_set_num_threads(16);
        /* Slice yz */
#pragma omp parallel
        {
#pragma omp for // Tested
            for (idx = 0; idx < 2097152; idx += 512) {
                u[idx]       *= boundryScale;
                u[idx + 128] *= boundryScale;
                u[idx + 256] *= boundryScale;
                u[idx + 384] *= boundryScale;
            }
#pragma omp for private(x, y, idx) // Tested
            for(z = 0; z < zMax; z++)
            {
                idx = z * zOffSet;
                for(y = 0; y < yMax; y++)
                {
                    idx++;
                    for( x = 1; x < xMax; x+= 8)
                    {
                        u[idx]     += u[idx - 1] * nu;
                        u[idx + 1] += u[idx] * nu;
                        u[idx + 2] += u[idx + 1] * nu;
                        u[idx + 3] += u[idx + 2] * nu;
                        u[idx + 4] += u[idx + 3] * nu;
                        u[idx + 5] += u[idx + 4] * nu;
                        u[idx + 6] += u[idx + 5] * nu;
                        u[idx + 7] += u[idx + 6] * nu;
                        idx += 8;
                    }
                    for( ;x < xMax; x++) {
                        u[idx] += u[idx - 1] * nu;
                        idx++;
                    }
                }
            }
#pragma omp for // Tested
            for (idx = 0; idx < 2097152; idx += 512) {
                u[idx]       *= boundryScale;
                u[idx + 128] *= boundryScale;
                u[idx + 256] *= boundryScale;
                u[idx + 384] *= boundryScale;
            }
            
            
#pragma omp for private(x, y, idx) // tested, not Optimized
            for(z = 0; z < zMax; z++)
            {
                for(y = 0; y < yMax; y++)
                {
                    idx = Index(xMax - 2, y, z);
                    for(x = xMax - 2; x >= 0; x-=2)
                    {
                        u[idx] += u[idx + 1] * nu;
                        u[idx - 1] += u[idx] * nu;
                        idx-=2;
                    }
                }
            }
            
            /* Slice xz */
            idx = 0;
#pragma omp for private(x, idx) // tested, not optimized
            for(z = 0; z < zMax; z++)
            {
                idx = z * zOffSet;
                for(x = 0; x < xMax; x+=4)
                {
                    u[idx] *= boundryScale;
                    u[idx + 1] *= boundryScale;
                    u[idx + 2] *= boundryScale;
                    u[idx + 3] *= boundryScale;
                    idx+=4;
                }
            }
            
            idx = 0;
#pragma omp for private(x, y, idx) // tested, not optimized
            for(z = 0; z < zMax; z++)
            {
                idx = z * zOffSet + xMax;
                for(y = 1; y < yMax; y++)
                {
                    for(x = 0; x < xMax; x++)
                    {
                        u[idx] += u[idx - xMax] * nu;
                        idx++;
                    }
                }
            }
            idx = 0;
#pragma omp for private(x, idx) // tested, not unrolled
            for(z = 0; z < zMax; z++)
            {
                idx = z * zOffSet;
                for(x = 0; x < xMax; x+=4)
                {
                    u[idx] *= boundryScale;
                    u[idx + 1] *= boundryScale;
                    u[idx + 2] *= boundryScale;
                    u[idx + 3] *= boundryScale;
                    idx+=4;
                }
            }
#pragma omp for private(x, y) // tested
            for(z = 0; z < zMax; z++)
            {
                for(y = yMax - 2; y >= 0; y--)
                {
                    idx = Index(0, y, z);
                    for(x = 0; x < xMax; x++)
                    {
                        u[idx] += u[idx + xMax] * nu;
                        idx++;
                    }
                }
            }
            /* Slice xy */
#pragma omp for // tested
            for(idx = 0; idx < zOffSet; idx+=4) {
                u[idx] *= boundryScale;
                u[idx+1] *= boundryScale;
                u[idx+2] *= boundryScale;
                u[idx+3] *= boundryScale;
                
            }
            
#pragma omp for private(x, z) // under testing
            for(y = 0; y < yMax; y++)
            {
                for(z = 1; z < zMax; z++)
                {
                    idx = Index(0, y, z);
                    for(x = 0; x < xMax; x++)
                    {
                        u[idx] = u[idx - zOffSet] * nu;
                        idx++;
                    }
                }
            }
#pragma omp for // tested
            for (idx = Index(0, 0, zMax - 1); idx < volume; idx+=4) {
                u[idx] *= boundryScale;
                u[idx + 1] *= boundryScale;
                u[idx + 2] *= boundryScale;
                u[idx + 3] *= boundryScale;
            }
#pragma omp for private(x, z)
            for(y = 0; y < yMax; y++)
            {
                for(z = zMax - 2; z >= 0; z--)
                {
                    idx = Index(0, y, z);
                    for(x = 0; x < xMax; x+=4)
                    {
                        offSetIdx = idx + zOffSet;
                        u[idx] += u[offSetIdx] * nu;
                        u[idx + 1] += u[offSetIdx + 1] * nu;
                        u[idx + 2] += u[offSetIdx + 2] * nu;
                        u[idx + 3] += u[offSetIdx + 3] * nu;
                    }
                }
            }
        }
        /* scale all */
        int i;
#pragma parallel omp for
        for (i = 0; i < volume; i += 8) {
            u[i]     *= postScale;
            u[i + 1] *= postScale;
            u[i + 2] *= postScale;
            u[i + 3] *= postScale;
            u[i + 4] *= postScale;
            u[i + 5] *= postScale;
            u[i + 6] *= postScale;
            u[i + 7] *= postScale;
        }
    }
}
void OMP_Deblur(double* u, const double* f, int maxIterations, double dt, double gamma, double sigma, double Ksigma)
{
    double epsilon = 1.0e-7;
    double sigma2 = SQR(sigma);
    int x, y, z, iteration, idx, idx2;
    int converged = 0;
    int lastConverged = 0;
    int fullyConverged = (xMax - 1) * (yMax - 1) * (zMax - 1);
    double* conv = OMP_conv;
    double* g = OMP_g;
    
    for(iteration = 0; iteration < maxIterations && converged != fullyConverged; iteration++)
    {
        omp_set_num_threads(16);
    #pragma omp parallel for private(y, x, idx, idx2)
        for(z = 1; z < zMax - 1; z++)
        {
            
            for(y = 1; y < yMax - 1; y++)
            {
                idx = Index(1, y, z);
                for(x = 1; x < xMax - 1; x+=2)
                {
                    idx2 = idx + 1;
                    g[idx] = 1.0 / sqrt(epsilon +
                                        SQR(u[idx] - u[idx + 1]) +
                                        SQR(u[idx] - u[idx - 1]) +
                                        SQR(u[idx] - u[idx + xMax]) +
                                        SQR(u[idx] - u[idx - xMax]) +
                                        SQR(u[idx] - u[idx + zOffSet]) +
                                        SQR(u[idx] - u[idx - zOffSet]));
                    g[idx2] = 1.0 / sqrt(epsilon +
                                        SQR(u[idx2] - u[idx2 + 1]) +
                                        SQR(u[idx2] - u[idx]) +
                                        SQR(u[idx2] - u[idx2 + xMax]) +
                                        SQR(u[idx2] - u[idx2 - xMax]) +
                                        SQR(u[idx2] - u[idx2 + zOffSet]) +
                                        SQR(u[idx2] - u[idx2 - zOffSet]));
                    idx += 2;
                }
            }
        }
        memcpy(conv, u, sizeof(double) * xMax * yMax * zMax);
        OMP_GaussianBlur(conv, Ksigma, 3);
        #pragma omp parallel for private(y, x, idx, idx2)
        for(z = 0; z < zMax; z++)
        {
            for(y = 0; y < yMax; y++)
            {
                idx = Index(0, y, z);
                for(x = 0; x < xMax; x+=2)
                {
                    idx2 = idx + 1;
                    double r = conv[idx] * f[idx] / sigma2;
                    r = (r * (2.38944 + r * (0.950037 + r))) / (4.65314 + r * (2.57541 + r * (1.48937 + r))); // can I simplify this?
                    conv[idx] -= f[idx] * r;
                    double r1 = conv[idx2] * f[idx2] / sigma2;
                    r1 = (r1 * (2.38944 + r1 * (0.950037 + r1))) / (4.65314 + r1 * (2.57541 + r1 * (1.48937 + r1))); // can I simplify this?
                    conv[idx2] -= f[idx2] * r1;
                    idx += 2;
                }
            }
        }
        OMP_GaussianBlur(conv, Ksigma, 3);
        converged = 0;
        
        double oldVal, newVal;
        double g1, g2, g3, g4, g5, g6;
        
        //int zz, yy, xx;
        
        
        for(z = 1; z < zMax - 1; z+=2)
        {
            for(y = 1; y < yMax - 1; y+=2)
            {
                for(x = 1; x < xMax - 1; x+=2)
                {
                    
                    // 1
                    idx = Index(x, y, z);
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                    
                    // 2
                    idx = idx + 1;
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                    
                    // 3
                    idx += 127;
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                    
                    // 4
                    idx += 16256;
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                    
                    // 5
                    
                    idx -= 16255;
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                    
                    // 6
                    idx += 16256;
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                    
                    // 7
                    idx += 127;
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                    
                    // 8
                    idx += 1;
                    g1 = g[idx - 1], g2 = g[idx + 1], g3 = g[idx - xMax], g4 = g[idx + xMax], g5 = g[idx - zOffSet], g6 = g[idx + zOffSet];
                    oldVal = u[idx];
                    newVal = (u[idx] + dt * (
                                             u[idx - 1] * g1 +
                                             u[idx + 1] * g2 +
                                             u[idx - xMax] * g3 +
                                             u[idx + xMax] * g4 +
                                             u[idx - zOffSet] * g5 +
                                             u[idx + zOffSet] * g6 - gamma * conv[idx])) /
                    (1.0 + dt * (g2 + g1 + g4 + g3 + g6 + g5));
                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[idx] = newVal;
                }
            }
        }
        
        
        if(converged > lastConverged)
        {
            printf("%d pixels have converged on iteration %d\n", converged, iteration);
            lastConverged = converged;
        }
    }
}

