#include "s21_tetris.h"

void create_field(field_settings* field) {
    for (int i = 0; i < field->height + 2; i++) {
        for (int j = 0; j < field->weight + 2; j++) {
            if (i == 0 || i == 21 || j == 0 || j == 11)
                printf("#");
            else {
                printf(" ");
            }
        }
    printf("\n");    
    }
}
