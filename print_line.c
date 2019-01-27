/* DDA Line Algorithm */
void print_line(struct Point p1,struct Point p2) {
  int dx = p2.x - p1.x;
  int dy = p2.y - p1.y;
  int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  
  float Xinc = dx / (float) steps;
  float Yinc = dy / (float) steps;
  
  float X = p1.x;
  float Y = p1.y;
  for (int i=0; i<=steps; i++){
	print_pixel(X,Y);
	X += Xinc;
	Y += Yinc;
  }
}

/* Algoritma Awal (Dari Bapake)
 * Kayaknya bentuk Interpretasi Bresenham Algorithm */
void print_line(struct Point p1,struct Point p2) {
  int dx = p2.x-p1.x;
  int dy = p2.y-p1.y;
  int control;
  int y = p1.y;
  int x = p1.x;
  if(dx>fabs(dy)){
    control = 2*dy - dx;
    if(dy<0){
      for(int i = p1.x; i < p2.x; i++){
        print_pixel(i,y);
        if(control > 0) {
          y--;
          control -= 2*dx;
        }
        control -= 2*dy;  
      }  
    }else {
      for(int i = p1.x; i < p2.x; i++){
        print_pixel(i,y);
        if(control > 0) {
          y++;
          control -= 2*dx;
        }
        control += 2*dy;
      }
    }
  } else {
    control = 2*dx - dy;
    if(dy<0){
      for(int i = p1.y; i > p2.y; i--){
        print_pixel(x,i);
        if(control > 0) {
          x++;
          control -= 2*dy;
        }
        control -= 2*dx;  
      }  
    }else {
      for(int i = p1.y; i < p2.y; i++){
        print_pixel(x,i);
        if(control > 0) {
          x++;
          control -= 2*dy;
        }
        control += 2*dx;
      }  
    }
  }  
}

/* Interpretasi lain dari Bresenham Algorithm */
void print_line(struct Point p1,struct Point p2) {
  int dx = abs(p2.x - p1.x);
  int dy = abs(p2.y - p1.y);
  int signx = p2.x > p1.x ? 1 : -1;
  int signy = p2.y > p1.y ? 1 : -1;
  int x = p1.x;
  int y = p1.y;
  int temp;
  int interchange = 0;
  int error;
  if (dy>dx){
	temp = dx;
	dx = dy;
	dy = temp;
	interchange = 1;
  }
  
  error = (2*dy) - dx;
  for (int i=0; i<dx; i++) {
	  if (error<0) {
		  if (interchange==1) {
			y = y + signy;
		  } else {
			x = x + signx;
		  }
		  error = error + (2*dy);
	  } else {
		  y = y + signy;
		  x = x + signx;
		  error = error + 2*(dy-dx);
	  }
	  print_pixel(x,y);
  }  
}
