#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  struct matrix * transform,
                  struct matrix * edges,
                  screen s) {

  FILE *f;
  struct matrix* mToAdd = NULL;
  char line[256];
  char parameters[256];
  double eToAdd[6];
  char axis;
  color c = {0,0,0};
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");

  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);

    if( !strcmp(line,"line") ){
      fgets(parameters,255,f);
      parameters[strlen(parameters)-1]='\0';

      sscanf(parameters,"%lf %lf %lf %lf %lf %lf",&eToAdd[0],&eToAdd[1],&eToAdd[2],&eToAdd[3],&eToAdd[4],&eToAdd[5]);

      add_edge(edges,eToAdd[0],eToAdd[1],eToAdd[2],eToAdd[3],eToAdd[4],eToAdd[5]);
    }

    else if( !strcmp(line,"ident") ){
      free_matrix(transform);

      transform = new_matrix(4,4);
      ident(transform);
    }

    else if( !strcmp(line,"scale")){
      fgets(parameters,255,f);
      parameters[strlen(parameters)-1]='\0';

      sscanf(parameters,"%lf %lf %lf",&eToAdd[0],&eToAdd[1],&eToAdd[2]);

      mToAdd = make_scale(eToAdd[0],eToAdd[1],eToAdd[2]);
      matrix_mult(mToAdd,transform);

      free_matrix(mToAdd);
    }

    else if( !strcmp(line,"move")){
      fgets(parameters,255,f);
      parameters[strlen(parameters)-1]='\0';

      sscanf(parameters,"%lf %lf %lf",&eToAdd[0],&eToAdd[1],&eToAdd[2]);

      mToAdd = make_translate(eToAdd[0],eToAdd[1],eToAdd[2]);

      matrix_mult(mToAdd,transform);

      free_matrix(mToAdd);
    }

    else if ( !strcmp(line,"rotate")){
      fgets(parameters,255,f);
      parameters[strlen(parameters)-1]='\0';

      sscanf(parameters,"%c %lf",&axis,&eToAdd[0]);

      eToAdd[0] *= (M_PI / 180);

      if(axis == 'x') mToAdd = make_rotX(eToAdd[0]);
      else if (axis == 'y') mToAdd = make_rotY(eToAdd[0]);
      else if (axis == 'z') mToAdd = make_rotZ(eToAdd[0]);

      matrix_mult(mToAdd,transform);

      free_matrix(mToAdd);
    }

    else if( !strcmp(line,"apply") ){
      matrix_mult(transform,edges);
    }

    else if( !strcmp(line,"display")){
      clear_screen(s);

      draw_lines(edges,s,c);

      display(s);
    }

    else if( !strcmp(line,"save")){
      fgets(parameters,255,f);
      parameters[strlen(parameters)-1]='\0';

      save_extension(s,parameters);
    }

    else if( !strcmp(line,"quit")){
      printf("End of script\n");
      break;
    }

    else {
      printf("Unknown command. Ending...\n");
      break;
    }
  }
}
