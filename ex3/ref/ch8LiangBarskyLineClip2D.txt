class wcPt2D 
{
   private:
      GLfloat x, y;

   public:
   /*  Default Constructor: initialize position as (0.0, 0.0).  */
   wcPt3D ( ) {
      x = y = 0.0;
   }

   setCoords (GLfloat xCoord, GLfloat yCoord) {
      x = xCoord;
      y = yCoord;
   }

   GLfloat getx ( ) const {
      return x;
   }

   GLfloat gety ( ) const {
      return y;
   }
};

inline GLint round (const GLfloat a)  { return GLint (a + 0.5); }

GLint clipTest (GLfloat p, GLfloat q, GLfloat * u1, GLfloat * u2)
{
  GLfloat r;
  GLint returnValue = true;

  if (p < 0.0) {
    r = q / p;
    if (r > *u2)
      returnValue = false;
    else
      if (r > *u1)
        *u1 = r;
  }
  else
    if (p > 0.0) {
      r = q / p;
      if (r < *u1)
        returnValue = false;
      else if (r < *u2)
        *u2 = r;
    }
    else
      /*  Thus p = 0 and line is parallel to clipping boundary.  */
      if (q < 0.0)
        /*  Line is outside clipping boundary.  */
        returnValue = false;

  return (returnValue);
}

void lineClipLiangBarsk (wcPt2D winMin, wcPt2D winMax, wcPt2D p1, wcPt2D p2)
{
  GLfloat u1 = 0.0, u2 = 1.0, dx = p2.getx ( ) - p1.getx ( ), dy;

  if (clipTest (-dx, p1.getx ( ) - winMin.getx ( ), &u1, &u2))
    if (clipTest (dx, winMax.getx ( ) - p1.getx ( ), &u1, &u2)) {
      dy = p2.gety ( ) - p1.gety ( );
      if (clipTest (-dy, p1.gety ( ) - winMin.gety ( ), &u1, &u2))
        if (clipTest (dy, winMax.gety ( ) - p1.gety ( ), &u1, &u2)) {
          if (u2 < 1.0) {
            p2.setCoords (p1.getx ( ) + u2 * dx, p1.gety ( ) + u2 * dy);
          }
          if (u1 > 0.0) {
            p1.setCoords (p1.getx ( ) + u1 * dx, p1.gety ( ) + u1 * dy);
          }
          lineBres (round (p1.getx ( )), round (p1.gety ( )), 
                       round (p2.getx ( )), round (p2.gety ( )));
        }
    }
}
