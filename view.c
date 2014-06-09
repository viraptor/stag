#include <string.h> // strlen
#include <stdio.h> // sprintf
// #include <wchar.h> // wchar, utf-8
#include <math.h> // ceil

#include "view.h"

// Implement when wide chars are actually used
/* void mvwadd_wchar(WINDOW *win, int y, int x, const int c) {
  // wchar mvwaddch, must convert to cchar_t for ncursesw
  cchar_t cc = {
    0,    // attribute
    {c,0} // cchar
  };
  mvwadd_wch(win, y, x, &cc);
  } */

void init_stag_win(stag_win_t *win, int height, int width, int y, int x) {
  // Initalize window struct
  win->win = newwin(height, width, y, x);
  win->x = x;
  win->y = y;
  win->width = width;
  win->height = height;
}

void format_axis_value(char *dest, float v) {
  // Populate dest with formatted value, e.g. 1000->1k, 1000000->1M
  char unitprefixes[9] = " KMGTPEZY";
  float remainder = v;

  // Special case for v < 1 and 0
  if(v < 0.0001) {
    sprintf(dest, "0");
    return;
  } else if(v < 1) {
    sprintf(dest, ".%.3f", v);
    return;
  }
  
  int i = 0;
  for(i = 0; i < 9 && remainder >= 1000; i++)
    remainder /= 1000;

  sprintf(dest, "%-.0f%c", remainder, unitprefixes[i]);
}

void draw_y_axis(stag_win_t *y_axis_win, int splits, float min, float max) {
  // Update y_axis with new line, splits and axis value labels
  wclear(y_axis_win->win);
  wrefresh(y_axis_win->win);
  
  // Draw axis line
  int i = 0;
  for(i = 0; i < y_axis_win->height; i++) {
    mvwaddch(y_axis_win->win, i, 0, ACS_VLINE);
  }
  mvwaddch(y_axis_win->win, 0, 0, ACS_LTEE);
  mvwaddch(y_axis_win->win, y_axis_win->height-1, 0, ACS_LTEE);

  // Draw axis values
  char axis_value[4];
  format_axis_value(&axis_value[0], max);
  mvwprintw(y_axis_win->win, 0, 2, "%s", axis_value);
  format_axis_value(&axis_value[0], min);
  mvwprintw(y_axis_win->win, y_axis_win->height-1, 2, "%s", axis_value);

  // Draw splits with axis values
  int split_height_step = floor(y_axis_win->height/(splits+1));
  int split_value_step = floor((max-min)/(splits+1));
  for(i = 0; i < splits; i++) {
    int split_v = (splits-i) * split_value_step;
    int split_height = (i+1) * split_height_step;
    mvwaddch(y_axis_win->win, split_height, 0, ACS_LTEE);
    format_axis_value(&axis_value[0], split_v);
    mvwprintw(y_axis_win->win, split_height, 2, "%s", axis_value);
  }

  
  wrefresh(y_axis_win->win);
}

int centered_x(stag_win_t *win, char *s) {
  // Return starting x to place text as centered
  int x = ((win->width)-strlen(s))/2;
  if(x < 0)
    return 0;

  return x;
}

void draw_title(stag_win_t *title_win, char *title) {
  // Draw title to window, centered and spaning multiple lines as needed
  int i = 0;
  int title_i = 0;
  int title_len = strlen(title);

  for(i = 0; i<title_win->height && title_i < title_len; i++) {
    char partial_title[title_win->width];
    strncpy(partial_title, title+title_i, title_win->width);
    int startx = centered_x(title_win, partial_title);
    mvwaddnstr(title_win->win, i, startx, partial_title, title_win->width);
    title_i += title_win->width;
  }
  wrefresh(title_win->win);
}

void draw_bar(stag_win_t *graph_win, int x, float v, int width, float min, float max) {
  // Draw new bar at position x according to value and graph scale

  // No widechars for now
  // wchar_t upper_half_block = L'\u2584';
  // wchar_t lower_half_block = L'\u2584';
  // wchar_t full_block = L'\u2589';

  // Fail conditions
  if(max-min == 0)
    return;
  if(x < 0)
    return;

  float height =  ceil((v-min)/(max-min) * graph_win->height);

  int j = 0;
  float i = 0;
  wattron(graph_win->win, A_REVERSE);
  for(j = 0; j < width; j++) {
    for(i = 0; i < height; i++)
      mvwaddch(graph_win->win, graph_win->height-1-i, x+j, ' ');
  }
  wattroff(graph_win->win,A_REVERSE);
}

void draw_graph_axis(stag_win_t *graph_win) {
  // Draw underline for the x axis
  int i = 0;
  wattron(graph_win->win, A_UNDERLINE);
  for(i = 0; i < graph_win->width; i++)
    mvwaddch(graph_win->win, graph_win->height-1, i, ' ');
  wattroff(graph_win->win, A_UNDERLINE);
}
