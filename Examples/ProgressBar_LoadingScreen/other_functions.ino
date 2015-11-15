#define deg_to_rad 0.01745 + 3.14159265
void DrawArc(int cx, int cy, int radius, int thick, int inc, int start, int stop)
{
  int Xo, Xi, Yo, Yi, lxo, lyo, lxi, lyi;

  for (int rotate = start; rotate >= stop; rotate -= (inc == 0 ? 1 : inc))
  {
    myGLCD.setColor(0xff00);

    float _cos = -cos(rotate * deg_to_rad);
    float _sin = sin(rotate * deg_to_rad);

    Xo = cx + (_cos * radius);
    //Xi = cx + (_cos * (radius - thick));

    Yo = cy + (_sin * radius);
    //Yi = cy + (_sin * (radius - thick));

    if (rotate <= start)
    {
      myGLCD.printNumI(start - rotate, Xo, Yo);
    }
  }
}
