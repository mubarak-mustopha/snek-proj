#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
void create_default_board(char **board){
    for (int i=0;i < 18;i++){
      board[i] = (char *) malloc(22 * sizeof(char));
    }

    strcpy(board[0], "####################\n");
    strcpy(board[1], "#                  #\n");
    strcpy(board[2], "# d>D    *         #\n");
    strcpy(board[3], "#                  #\n");
    strcpy(board[4], "#                  #\n");
    strcpy(board[5], "#                  #\n");
    strcpy(board[6], "#                  #\n");
    strcpy(board[7], "#                  #\n");
    strcpy(board[8], "#                  #\n");
    strcpy(board[9], "#                  #\n");
    strcpy(board[10], "#                  #\n");
    strcpy(board[11], "#                  #\n");
    strcpy(board[12], "#                  #\n");
    strcpy(board[13], "#                  #\n");
    strcpy(board[14], "#                  #\n");
    strcpy(board[16], "#                  #\n");
    strcpy(board[15], "#                  #\n");
    strcpy(board[17], "####################\n");
}

game_t *create_default_game() {
  // TODO: Implement this function.
  game_t *game = (game_t *) malloc(sizeof(game_t));
  game->num_rows = 18;
  game->board = (char **) malloc(18 * sizeof(char *));
  game->num_snakes = 1;
  game->snakes = (snake_t *) malloc(sizeof(snake_t));
  
  snake_t *snake = game->snakes;
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->live = true;

  create_default_board(game->board);

  return game;
}

/* Task 2 */
void free_game(game_t *game) {
  // TODO: Implement this function.
  free(game->snakes);
  for (int i = 0;i < game->num_rows; i++){
    free(game->board[i]);
  }
  free(game->board);
  free(game);

  // return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  // TODO: Implement this function.
  for (int i=0;i < game->num_rows;i++){
    fprintf(fp, "%s", game->board[i]);
  }
  // return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  switch(c){
    case 'w': case 'a': case 's': case 'd': {
      return true;
    }
    default: 
      return false;
  }
  
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  switch(c){
    case 'W': case 'A': case 'S': case 'D': {
      return true;
    }
    default: 
      return false;
  }
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  char *ptr = strchr("wasd^<v>WASDx",c);
  if (ptr == NULL) return false;
  return true;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c) {
    case '^':
      return 'w';

    case '<':
      return 'a';

    case 'v':
      return 's';
      
    case '>':
      return 'd';    
    
    default:
      return '?';
  }
  
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
    switch(c) {
    case 'W':
      return '^';

    case 'A':
      return '<';

    case 'S':
      return 'v';
      
    case 'D':
      return '>';    
    
    default:
      return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S'){
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W'){
    return cur_row - 1;
  } else{
  return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D'){
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A'){
    return cur_col - 1;
  } else{
  return cur_col;
  }
}

/*
 Returns 1 if snake_a should have a higher numbering than snake_b
  i.e the tail of snake_a is in a higher coordinate
  Returns -1 if the reverse is the case
*/
int cmp_snakes(snake_t snake_a, snake_t snake_b){
  if(snake_a.tail_row > snake_b.tail_row){
    return 1;
  } else if (snake_a.tail_row < snake_b.tail_row){
    return -1;
  } else {
      if(snake_a.tail_col > snake_b.tail_col){
          return 1;
        } else{
          return -1;
        }
  }
}

void sort_game_snakes(game_t *game){
  // bubble sort
  bool swap;
  for (int i=0; i< game->num_snakes;i++){
    swap = false;
    for(int j=0; j < game->num_snakes -i - 1; j++ ){

      if (cmp_snakes(game->snakes[j], game->snakes[j+1]) == 1){
        // i.e jth snake should have a higher numbering than (j+1)th snake
        snake_t temp_snake = game->snakes[j];
        game->snakes[j] = game->snakes[j+1];
        game->snakes[j+1] = temp_snake;
        swap = true;
      }

    }
    if (!swap)
      break;
  }
}



/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
    // sort game snakes by tail coords
  sort_game_snakes(game);

  snake_t snake = game->snakes[snum];

  char head = game->board[snake.head_row][snake.head_col];

  int next_col = get_next_col(snake.head_col, head);
  int next_row = get_next_row(snake.head_row, head);

  return game->board[next_row][next_col];
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  // TODO: Implement this function.
  
  // return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/* Task 5.2 */
game_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {
  // TODO: Implement this function.
  return NULL;
}
