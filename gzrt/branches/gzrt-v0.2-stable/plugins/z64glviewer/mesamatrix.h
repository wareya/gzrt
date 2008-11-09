/*

Some part of this file is based on the mesa core which has
the following license.

Copyright (C) 1999-2003  Brian Paul   All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <math.h>

#define VALPI 3.141592654
#define DEG2RAD 2*VALPI/360.0

#define SWAP_ROWS(a, b) { GLfloat *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

#define m4_00(v) v[0]
#define m4_01(v) v[4]
#define m4_02(v) v[8]
#define m4_03(v) v[12]
#define m4_10(v) v[1]
#define m4_11(v) v[5]
#define m4_12(v) v[9]
#define m4_13(v) v[13]
#define m4_20(v) v[2]
#define m4_21(v) v[6]
#define m4_22(v) v[10]
#define m4_23(v) v[14]
#define m4_30(v) v[3]
#define m4_31(v) v[7]
#define m4_32(v) v[11]
#define m4_33(v) v[15]

#define m3_00(v) v[0]
#define m3_01(v) v[3]
#define m3_02(v) v[6]
#define m3_10(v) v[1]
#define m3_11(v) v[4]
#define m3_12(v) v[7]
#define m3_20(v) v[2]
#define m3_21(v) v[5]
#define m3_22(v) v[8]

#define x(v) v[0]
#define y(v) v[1]
#define z(v) v[2]
#define w(v) v[3]

void multM4Vect(float vr[], float mat[], float v[])
{
    float temp[4];
	temp[0] = v[0]*mat[0] + v[1]*mat[4] + v[2]*mat[8] + v[3]*mat[12];
	temp[1] = v[0]*mat[1] + v[1]*mat[5] + v[2]*mat[9] + v[3]*mat[13];
	temp[2] = v[0]*mat[2] + v[1]*mat[6] + v[2]*mat[10] + v[3]*mat[14];
	temp[3] = v[0]*mat[3] + v[1]*mat[7] + v[2]*mat[11] + v[3]*mat[15];
    vr[0]=temp[0];
    vr[1]=temp[1];
    vr[2]=temp[2];
    vr[3]=temp[3];
    
}

void addM4(float A[], float B[])
{
    m4_00(A) += m4_00(B);
    m4_10(A) += m4_10(B);
    m4_20(A) += m4_20(B);
    m4_30(A) += m4_30(B);
    m4_01(A) += m4_01(B);
    m4_11(A) += m4_11(B);
    m4_21(A) += m4_21(B);
    m4_31(A) += m4_31(B);
    m4_02(A) += m4_02(B);
    m4_12(A) += m4_12(B);
    m4_22(A) += m4_22(B);
    m4_32(A) += m4_32(B);
    m4_03(A) += m4_03(B);
    m4_13(A) += m4_13(B);
    m4_23(A) += m4_23(B);
    m4_33(A) += m4_33(B);
}

void addM3(float A[], float B[])
{
    m3_00(A) += m3_00(B);
    m3_10(A) += m3_10(B);
    m3_20(A) += m3_20(B);
    m3_01(A) += m3_01(B);
    m3_11(A) += m3_11(B);
    m3_21(A) += m3_21(B);
    m3_02(A) += m3_02(B);
    m3_12(A) += m3_12(B);
    m3_22(A) += m3_22(B);
}

void multM4(float C[], float A[], float B[])
{
    m4_00(C)= m4_00(A) * m4_00(B) + m4_01(A) * m4_10(B) + m4_02(A) * m4_20(B) + m4_03(A) * m4_30(B);
    m4_10(C)= m4_10(A) * m4_00(B) + m4_11(A) * m4_10(B) + m4_12(A) * m4_20(B) + m4_13(A) * m4_30(B);
    m4_20(C)= m4_20(A) * m4_00(B) + m4_21(A) * m4_10(B) + m4_22(A) * m4_20(B) + m4_23(A) * m4_30(B);
    m4_30(C)= m4_30(A) * m4_00(B) + m4_31(A) * m4_10(B) + m4_32(A) * m4_20(B) + m4_33(A) * m4_30(B);
    m4_01(C)= m4_00(A) * m4_01(B) + m4_01(A) * m4_11(B) + m4_02(A) * m4_21(B) + m4_03(A) * m4_31(B);
    m4_11(C)= m4_10(A) * m4_01(B) + m4_11(A) * m4_11(B) + m4_12(A) * m4_21(B) + m4_13(A) * m4_31(B);
    m4_21(C)= m4_20(A) * m4_01(B) + m4_21(A) * m4_11(B) + m4_22(A) * m4_21(B) + m4_23(A) * m4_31(B);
    m4_31(C)= m4_30(A) * m4_01(B) + m4_31(A) * m4_11(B) + m4_32(A) * m4_21(B) + m4_33(A) * m4_31(B);
    m4_02(C)= m4_00(A) * m4_02(B) + m4_01(A) * m4_12(B) + m4_02(A) * m4_22(B) + m4_03(A) * m4_32(B);
    m4_12(C)= m4_10(A) * m4_02(B) + m4_11(A) * m4_12(B) + m4_12(A) * m4_22(B) + m4_13(A) * m4_32(B);
    m4_22(C)= m4_20(A) * m4_02(B) + m4_21(A) * m4_12(B) + m4_22(A) * m4_22(B) + m4_23(A) * m4_32(B);
    m4_32(C)= m4_30(A) * m4_02(B) + m4_31(A) * m4_12(B) + m4_32(A) * m4_22(B) + m4_33(A) * m4_32(B);
    m4_03(C)= m4_00(A) * m4_03(B) + m4_01(A) * m4_13(B) + m4_02(A) * m4_23(B) + m4_03(A) * m4_33(B);
    m4_13(C)= m4_10(A) * m4_03(B) + m4_11(A) * m4_13(B) + m4_12(A) * m4_23(B) + m4_13(A) * m4_33(B);
    m4_23(C)= m4_20(A) * m4_03(B) + m4_21(A) * m4_13(B) + m4_22(A) * m4_23(B) + m4_23(A) * m4_33(B);
    m4_33(C)= m4_30(A) * m4_03(B) + m4_31(A) * m4_13(B) + m4_32(A) * m4_23(B) + m4_33(A) * m4_33(B);
}

void mult(float C[], float A[], float B[])
{
    m3_00(C) = m3_00(A) * m3_00(B) + m3_01(A) * m3_10(B) + m3_02(A) * m3_20(B);
    m3_10(C) = m3_10(A) * m3_00(B) + m3_11(A) * m3_10(B) + m3_12(A) * m3_20(B);
    m3_20(C) = m3_20(A) * m3_00(B) + m3_21(A) * m3_10(B) + m3_22(A) * m3_20(B);
    m3_01(C) = m3_00(A) * m3_01(B) + m3_01(A) * m3_11(B) + m3_02(A) * m3_21(B);
    m3_11(C) = m3_10(A) * m3_01(B) + m3_11(A) * m3_11(B) + m3_12(A) * m3_21(B);
    m3_21(C) = m3_20(A) * m3_01(B) + m3_21(A) * m3_11(B) + m3_22(A) * m3_21(B);
    m3_02(C) = m3_00(A) * m3_02(B) + m3_01(A) * m3_12(B) + m3_02(A) * m3_22(B);
    m3_12(C) = m3_10(A) * m3_02(B) + m3_11(A) * m3_12(B) + m3_12(A) * m3_22(B);
    m3_22(C) = m3_20(A) * m3_02(B) + m3_21(A) * m3_12(B) + m3_22(A) * m3_22(B);
}


/*
 * Compute inverse of 4x4 transformation matrix.
 * Code contributed by Jacques Leroy jle@star.be
 * Return GL_TRUE for success, GL_FALSE for failure (singular matrix)
 */


static GLboolean invert_matrix_general( GLfloat *min, GLfloat *mout )
{
   const GLfloat *m = min;
   GLfloat *out = mout;
   GLfloat wtmp[4][8];
   GLfloat m0, m1, m2, m3, s;
   GLfloat *r0, *r1, *r2, *r3;

   r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

   r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
   r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
   r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

   r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
   r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
   r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

   r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
   r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
   r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

   r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
   r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
   r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

   /* choose pivot - or die */
   if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
   if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
   if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
   if (0.0 == r0[0])  return GL_FALSE;

   /* eliminate first variable     */
   m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
   s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
   s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
   s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
   s = r0[4];
   if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
   s = r0[5];
   if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
   s = r0[6];
   if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
   s = r0[7];
   if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

   /* choose pivot - or die */
   if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
   if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
   if (0.0 == r1[1])  return GL_FALSE;

   /* eliminate second variable */
   m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
   r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
   r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
   s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
   s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
   s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
   s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

   /* choose pivot - or die */
   if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
   if (0.0 == r2[2])  return GL_FALSE;

   /* eliminate third variable */
   m3 = r3[2]/r2[2];
   r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
   r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
   r3[7] -= m3 * r2[7];

   /* last check */
   if (0.0 == r3[3]) return GL_FALSE;

   s = 1.0F/r3[3];             /* now back substitute row 3 */
   r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

   m2 = r2[3];                 /* now back substitute row 2 */
   s  = 1.0F/r2[2];
   r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
   r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
   m1 = r1[3];
   r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
   r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
   m0 = r0[3];
   r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
   r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

   m1 = r1[2];                 /* now back substitute row 1 */
   s  = 1.0F/r1[1];
   r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
   r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
   m0 = r0[2];
   r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
   r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

   m0 = r0[1];                 /* now back substitute row 0 */
   s  = 1.0F/r0[0];
   r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
   r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

   MAT(out,0,0) = r0[4]; MAT(out,0,1) = r0[5],
   MAT(out,0,2) = r0[6]; MAT(out,0,3) = r0[7],
   MAT(out,1,0) = r1[4]; MAT(out,1,1) = r1[5],
   MAT(out,1,2) = r1[6]; MAT(out,1,3) = r1[7],
   MAT(out,2,0) = r2[4]; MAT(out,2,1) = r2[5],
   MAT(out,2,2) = r2[6]; MAT(out,2,3) = r2[7],
   MAT(out,3,0) = r3[4]; MAT(out,3,1) = r3[5],
   MAT(out,3,2) = r3[6]; MAT(out,3,3) = r3[7];

   return GL_TRUE;
}
#undef SWAP_ROWS


void matrixTranslate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z )
{
   m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
   m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
   m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
   m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];
}

/**
 * Identity matrix.
 */
static GLfloat Identity[16] = {
   1.0, 0.0, 0.0, 0.0,
   0.0, 1.0, 0.0, 0.0,
   0.0, 0.0, 1.0, 0.0,
   0.0, 0.0, 0.0, 1.0
};

void matrixRotate( GLfloat *mat, GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
   GLfloat m[16];
   GLfloat tempm[16];
   GLboolean optimized;

   s = (GLfloat) sinf( angle * DEG2RAD );
   c = (GLfloat) cosf( angle * DEG2RAD );

   memcpy(m, Identity, sizeof(GLfloat)*16);
   memcpy(tempm, mat, sizeof(GLfloat)*16);
   optimized = GL_FALSE;

#define M(row,col)  m[col*4+row]

   if (x == 0.0F) {
      if (y == 0.0F) {
         if (z != 0.0F) {
            optimized = GL_TRUE;
            /* rotate only around z-axis */
            M(0,0) = c;
            M(1,1) = c;
            if (z < 0.0F) {
               M(0,1) = s;
               M(1,0) = -s;
            }
            else {
               M(0,1) = -s;
               M(1,0) = s;
            }
         }
      }
      else if (z == 0.0F) {
         optimized = GL_TRUE;
         /* rotate only around y-axis */
         M(0,0) = c;
         M(2,2) = c;
         if (y < 0.0F) {
            M(0,2) = -s;
            M(2,0) = s;
         }
         else {
            M(0,2) = s;
            M(2,0) = -s;
         }
      }
   }
   else if (y == 0.0F) {
      if (z == 0.0F) {
         optimized = GL_TRUE;
         /* rotate only around x-axis */
         M(1,1) = c;
         M(2,2) = c;
         if (x < 0.0F) {
            M(1,2) = s;
            M(2,1) = -s;
         }
         else {
            M(1,2) = -s;
            M(2,1) = s;
         }
      }
   }

   if (!optimized) {
      const GLfloat mag = sqrtf(x * x + y * y + z * z);

      if (mag <= 1.0e-4) {
         /* no rotation, leave mat as-is */
         return;
      }

      x /= mag;
      y /= mag;
      z /= mag;


      /*
       *     Arbitrary axis rotation matrix.
       *
       *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
       *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
       *  (which is about the X-axis), and the two composite transforms
       *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
       *  from the arbitrary axis to the X-axis then back.  They are
       *  all elementary rotations.
       *
       *  Rz' is a rotation about the Z-axis, to bring the axis vector
       *  into the x-z plane.  Then Ry' is applied, rotating about the
       *  Y-axis to bring the axis vector parallel with the X-axis.  The
       *  rotation about the X-axis is then performed.  Ry and Rz are
       *  simply the respective inverse transforms to bring the arbitrary
       *  axis back to it's original orientation.  The first transforms
       *  Rz' and Ry' are considered inverses, since the data from the
       *  arbitrary axis gives you info on how to get to it, not how
       *  to get away from it, and an inverse must be applied.
       *
       *  The basic calculation used is to recognize that the arbitrary
       *  axis vector (x, y, z), since it is of unit length, actually
       *  represents the sines and cosines of the angles to rotate the
       *  X-axis to the same orientation, with theta being the angle about
       *  Z and phi the angle about Y (in the order described above)
       *  as follows:
       *
       *  cos ( theta ) = x / sqrt ( 1 - z^2 )
       *  sin ( theta ) = y / sqrt ( 1 - z^2 )
       *
       *  cos ( phi ) = sqrt ( 1 - z^2 )
       *  sin ( phi ) = z
       *
       *  Note that cos ( phi ) can further be inserted to the above
       *  formulas:
       *
       *  cos ( theta ) = x / cos ( phi )
       *  sin ( theta ) = y / sin ( phi )
       *
       *  ...etc.  Because of those relations and the standard trigonometric
       *  relations, it is pssible to reduce the transforms down to what
       *  is used below.  It may be that any primary axis chosen will give the
       *  same results (modulo a sign convention) using thie method.
       *
       *  Particularly nice is to notice that all divisions that might
       *  have caused trouble when parallel to certain planes or
       *  axis go away with care paid to reducing the expressions.
       *  After checking, it does perform correctly under all cases, since
       *  in all the cases of division where the denominator would have
       *  been zero, the numerator would have been zero as well, giving
       *  the expected result.
       */

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * s;
      ys = y * s;
      zs = z * s;
      one_c = 1.0F - c;

      /* We already hold the identity-matrix so we can skip some statements */
      M(0,0) = (one_c * xx) + c;
      M(0,1) = (one_c * xy) - zs;
      M(0,2) = (one_c * zx) + ys;
/*    M(0,3) = 0.0F; */

      M(1,0) = (one_c * xy) + zs;
      M(1,1) = (one_c * yy) + c;
      M(1,2) = (one_c * yz) - xs;
/*    M(1,3) = 0.0F; */

      M(2,0) = (one_c * zx) - ys;
      M(2,1) = (one_c * yz) + xs;
      M(2,2) = (one_c * zz) + c;
/*    M(2,3) = 0.0F; */

/*
      M(3,0) = 0.0F;
      M(3,1) = 0.0F;
      M(3,2) = 0.0F;
      M(3,3) = 1.0F;
*/
   }
#undef M

   multM4( mat, tempm, m);
}

void matrixLookAt(GLfloat *m, GLfloat eyex, GLfloat eyey, GLfloat eyez,
      GLfloat centerx, GLfloat centery, GLfloat centerz,
      GLfloat upx, GLfloat upy, GLfloat upz)
{
   GLfloat x[3], y[3], z[3];
   GLfloat mag;

   /* Make rotation matrix */

   /* Z vector */
   z[0] = eyex - centerx;
   z[1] = eyey - centery;
   z[2] = eyez - centerz;
   mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
   if (mag) {			/* mpichler, 19950515 */
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
   }

   /* Y vector */
   y[0] = upx;
   y[1] = upy;
   y[2] = upz;

   /* X vector = Y cross Z */
   x[0] = y[1] * z[2] - y[2] * z[1];
   x[1] = -y[0] * z[2] + y[2] * z[0];
   x[2] = y[0] * z[1] - y[1] * z[0];

   /* Recompute Y = Z cross X */
   y[0] = z[1] * x[2] - z[2] * x[1];
   y[1] = -z[0] * x[2] + z[2] * x[0];
   y[2] = z[0] * x[1] - z[1] * x[0];

   /* mpichler, 19950515 */
   /* cross product gives area of parallelogram, which is < 1.0 for
    * non-perpendicular unit-length vectors; so normalize x, y here
    */

   mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
   if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
   }

   mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
   if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
   }

#define M(row,col)  m[col*4+row]
   M(0, 0) = x[0];
   M(0, 1) = x[1];
   M(0, 2) = x[2];
   M(0, 3) = 0.0;
   M(1, 0) = y[0];
   M(1, 1) = y[1];
   M(1, 2) = y[2];
   M(1, 3) = 0.0;
   M(2, 0) = z[0];
   M(2, 1) = z[1];
   M(2, 2) = z[2];
   M(2, 3) = 0.0;
   M(3, 0) = 0.0;
   M(3, 1) = 0.0;
   M(3, 2) = 0.0;
   M(3, 3) = 1.0;
#undef M
    matrixTranslate(m,-eyex,-eyey,-eyez);
}

void frustum(GLfloat *m, GLfloat left, GLfloat right,
                 GLfloat bottom, GLfloat top,
                 GLfloat nearval, GLfloat farval)
{
   GLfloat x, y, a, b, c, d;

   x = (2.0 * nearval) / (right - left);
   y = (2.0 * nearval) / (top - bottom);
   a = (right + left) / (right - left);
   b = (top + bottom) / (top - bottom);
   c = -(farval + nearval) / ( farval - nearval);
   d = -(2.0 * farval * nearval) / (farval - nearval);

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M
}

void matrixPerspective(GLfloat *m, GLfloat fovy, GLfloat aspect,
                           GLfloat zNear, GLfloat zFar)
{
   GLfloat xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovy * VALPI / 360.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

   /* don't call glFrustum() because of error semantics (covglu) */
   frustum(m, xmin, xmax, ymin, ymax, zNear, zFar);
}

