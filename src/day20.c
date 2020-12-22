#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day20.h>
#include <utils.h>

#define TILE_ASCII_SIZE 122
#define TOP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3

typedef struct {
    u16 edges[4]; // top right bottom left
    u8 spin_count;
    u8 flipped;
} Tile;

u16 u10_flip_bits(u16 i) {
    // Like flipping a u16, except we shift right by 6 at the end
    i = i >> 8 | i << 8;
    i = (i & 0xF0F0) >> 4 | (i & 0x0F0F) << 4;
    i = (i & 0xCCCC) >> 2 | (i & 0x3333) << 2;
    i = (i & 0xAAAA) >> 1 | (i & 0x5555) << 1;
    return i >> 6;
}

void spin_tile(Tile *tile) {
    // rotate clockwise
    u16 temp = tile->edges[LEFT];
    tile->edges[LEFT] = tile->edges[BOTTOM];
    tile->edges[BOTTOM] = tile->edges[RIGHT];
    tile->edges[RIGHT] = tile->edges[TOP];
    tile->edges[TOP] = temp;
    tile->spin_count = (tile->spin_count + 1) % 4;
}

void flip_tile(Tile *tile) {
    // flip around the horizontal axis
    u16 temp = tile->edges[BOTTOM];
    tile->edges[BOTTOM] = u10_flip_bits(tile->edges[TOP]);
    tile->edges[TOP] = u10_flip_bits(temp);
    tile->edges[RIGHT] = u10_flip_bits(tile->edges[RIGHT]);
    tile->edges[LEFT] = u10_flip_bits(tile->edges[LEFT]);
    tile->flipped ^= 1;
}

u16 get_tile_id(const u8* input, u16 tile_index) {
    input += TILE_ASCII_SIZE * tile_index + 5; 
    return skip_atoi(&input);
}

void draw_tile(const u8* input, u8* output, u16 tile_index, Tile *tile, u16 square_pixel_len) {
    input += TILE_ASCII_SIZE * tile_index + 23;
    s16 next_col_offset;
    s16 next_line_offset;
    if (tile->flipped) {
        if (tile->spin_count == 0) {
            output += 7 * square_pixel_len; // start at the bottom left
            next_col_offset = 1;
            next_line_offset = - square_pixel_len - 8; // return at the start of the previous line
        } else if (tile->spin_count == 1) {
            next_col_offset = square_pixel_len; // next line
            next_line_offset = -8 * square_pixel_len + 1; // return to the start of the next column
        } else if (tile->spin_count == 2) {
            output += 7; // start at top right
            next_col_offset = -1;
            next_line_offset = square_pixel_len + 8;
        } else {
            output += 8 * square_pixel_len - (square_pixel_len - 8) - 1; // start at the bottom right
            next_col_offset = -square_pixel_len; // previous line
            next_line_offset = 8 * square_pixel_len - 1; // return to the end of the previous column
        }
    } else {
        if (tile->spin_count == 0) {
            next_col_offset = 1;
            next_line_offset = square_pixel_len - 8;
        } else if (tile->spin_count == 1) {
            output += 7; // start at top right
            next_col_offset = square_pixel_len; // next line
            next_line_offset = -8 * square_pixel_len - 1; // return to the start of the previous column
        } else if (tile->spin_count == 2) {
            output += 8 * square_pixel_len - (square_pixel_len - 8) - 1; // start at the bottom right
            next_col_offset = -1;
            next_line_offset = -(square_pixel_len - 8);
        } else {
            output += 7 * square_pixel_len; // start at the bottom left
            next_col_offset = -square_pixel_len; // previous line
            next_line_offset = 8 * square_pixel_len + 1; // return to the end of the next column
        }
    }
    
    for (u16 row = 0; row < 8; row++) {
        for (u16 col = 0; col < 8; col++) {
            *output = *input == '#';
            input++;
            output += next_col_offset;
        }
        input += 3; // right border + newline + left border
        output += next_line_offset;
    }
}

void mirror_rows(u8 *input, u16 square_pixel_len) {
    u16 row_i = 0;
    for (u16 row = 0; row < square_pixel_len; row++) {
        u16 i = row_i;
        for (u16 col = 0; col < square_pixel_len / 2; col++) {
            u8 temp = input[i];
            input[i] = input[row_i + square_pixel_len - 1 - col];
            input[row_i + square_pixel_len - 1 - col] = temp;
            i++;
        }
        row_i += square_pixel_len;
    }
}

void rotate(u8 *input, u16 square_pixel_len) {
    for (u16 n = 0; n < square_pixel_len - 1; n++) {
        for (u16 m = n + 1; m < square_pixel_len; m++) {
            u16 idx1 = n * square_pixel_len + m;
            u16 idx2 = m * square_pixel_len + n;
            u8 temp = input[idx1];
            input[idx1] = input[idx2];
            input[idx2] = temp;
        }
    }
    mirror_rows(input, square_pixel_len);
}

bool check_for_monsters(u8* input, u16 square_pixel_len) {
    const u8 pattern[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1,
        0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0,
    };
    const u16 pattern_col_count = 20;
    const u16 pattern_row_count = 3;
    bool monsters_found = FALSE;
    u16 base_i = 0;
    for (u16 base_row = 0; base_row < square_pixel_len - (pattern_row_count - 1); base_row++) {
        for (u16 base_col = 0; base_col < square_pixel_len - (pattern_col_count - 1); base_col++) {
            u16 pattern_i = 0;
            u16 i = base_i;
            for (u16 pattern_row = 0; pattern_row < pattern_row_count; pattern_row++) {
                for (u16 pattern_col = 0; pattern_col < pattern_col_count; pattern_col++) {
                    if (pattern[pattern_i] && !input[i]) {
                        goto pattern_not_matching;
                    }
                    pattern_i++;
                    i++;
                }
                i += square_pixel_len - pattern_col_count;
            }
            monsters_found = TRUE;
            pattern_i = 0;
            i = base_i;
            for (u16 pattern_row = 0; pattern_row < pattern_row_count; pattern_row++) {
                for (u16 pattern_col = 0; pattern_col < pattern_col_count; pattern_col++) {
                    if (pattern[pattern_i]) {
                        input[i] = 2;
                    }
                    pattern_i++;
                    i++;
                }
                i += square_pixel_len - pattern_col_count;
            }
pattern_not_matching:
            base_i++;
        }
        base_i += pattern_col_count - 1;
    }
    return monsters_found;
}

u16 count_remaining_1s(u8* input, u16 square_pixel_len) {
    u16 count = square_pixel_len * square_pixel_len;
    u16 ret = 0;
    for (u16 i = 0; i < count; i++) {
        if (input[i] == 1) {
            ret++;
        }
    }
    return ret;
}

s16 part2_count(u8* input, u16 square_pixel_len) {
    for (u16 i = 0; i < 4; i++) {
        if (check_for_monsters(input, square_pixel_len)) {
            return count_remaining_1s(input, square_pixel_len);
        }
        rotate(input, square_pixel_len);
    }
    mirror_rows(input, square_pixel_len);
    for (u16 i = 0; i < 4; i++) {
        if (check_for_monsters(input, square_pixel_len)) {
            return count_remaining_1s(input, square_pixel_len);
        }
        rotate(input, square_pixel_len);
    }
    return -1;
}

void day20() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    u16 tile_count = sizeof DAY20_INPUT / TILE_ASCII_SIZE;
    u16 square_len = 0; // sqrt(tile_count)
    for (u16 i = 1; i < tile_count; i++) {
        square_len = tile_count / i;
        if (square_len == i) {
            break;
        }
    }
    Tile *tiles = MEM_alloc(tile_count * sizeof (Tile));
    u8 faces_count[1024] = {0}; // for each of every possible face, how many are present? (0, 1 or 2)
    s16 (*faces_to_tile_idx)[2] = MEM_alloc(1024 * sizeof (u16[2])); // which tile index (not ID!) do they belong to ?
    memsetU16((u16*) faces_to_tile_idx, -1,  2 * 1024);
    const u8 *tile_ascii_ptr = DAY20_INPUT;
    for (u16 i = 0; i < tile_count; i++) {
        const u8 *cursor;
        tiles[i].spin_count = 0;
        tiles[i].flipped = FALSE;
        u16 acc;
        // top
        cursor = tile_ascii_ptr + 11;
        acc = 0;
        for (u16 bit = 0; bit < 10; bit++) {
            acc <<= 1;
            acc += *cursor++ == '#';
        }
        tiles[i].edges[TOP] = acc;
        // bottom
        cursor = tile_ascii_ptr + 119;
        acc = 0;
        for (u16 bit = 0; bit < 10; bit++) {
            acc <<= 1;
            acc += *cursor-- == '#';
        }
        tiles[i].edges[BOTTOM] = acc;
        // left
        cursor = tile_ascii_ptr + 110;
        acc = 0;
        for (u16 bit = 0; bit < 10; bit++) {
            acc <<= 1;
            acc += *cursor == '#';
            cursor -= 11;
        }
        tiles[i].edges[LEFT] = acc;
        // right
        cursor = tile_ascii_ptr + 20;
        acc = 0;
        for (u16 bit = 0; bit < 10; bit++) {
            acc <<= 1;
            acc += *cursor == '#';
            cursor += 11;
        }
        tiles[i].edges[RIGHT] = acc;
        for (u16 f = 0; f < 4; f++) {
            u16 face = tiles[i].edges[f];
            if (faces_count[face]++ == 0) {
                // that was the first tile to have this particular face
                faces_to_tile_idx[face][0] = i;
            } else {
                // another tile had this face, use the second slot
                faces_to_tile_idx[face][1] = i;
            }
        }
        tile_ascii_ptr += TILE_ASCII_SIZE;
    }

    u16 *aranged_tiles_indexes = MEM_alloc(tile_count * sizeof (u16));

    // Select the top left corner, it's the first tile we find that has 2 faces that aren't seen anywhere else
    for (u16 i = 0; i < tile_count; i++) {
        u16 unique_face_count = 0;
        u16 unique_faces[2];
        for (u16 f = 0; f < 4; f++) {
            u16 face = tiles[i].edges[f];
            if (faces_count[face] + faces_count[u10_flip_bits(face)] == 1) {
                unique_faces[unique_face_count++] = face;
            }
        }
        if (unique_face_count == 2) {
            // This is a corner, select it as top left corner
            aranged_tiles_indexes[0] = i;
            // If needed, spin it so the 2 unique faces are at the top and the left
            while ((unique_faces[0] != tiles[i].edges[TOP] && unique_faces[1] != tiles[i].edges[TOP]) || (unique_faces[0] != tiles[i].edges[LEFT] && unique_faces[1] != tiles[i].edges[LEFT])) {
                spin_tile(&tiles[i]);
            }
            break;
        }
    }

    // Place the first line
    for (u16 i = 1; i < square_len; i++) {
        u16 previous_tile_index = aranged_tiles_indexes[i - 1];
        u16 face_to_match = tiles[previous_tile_index].edges[RIGHT];
        s16 selected_tile_index = faces_to_tile_idx[face_to_match][0];
        if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
            selected_tile_index = faces_to_tile_idx[face_to_match][1];
            if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
                u16 face_to_match_flipped = u10_flip_bits(face_to_match);
                selected_tile_index = faces_to_tile_idx[face_to_match_flipped][0];
                if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
                    selected_tile_index = faces_to_tile_idx[face_to_match_flipped][1];
                    if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
                        SYS_die("Couldn't find the next tile");
                    }
                }
            }
        }
        aranged_tiles_indexes[i] = selected_tile_index;
        for (u16 arangement = 0; arangement < 8; arangement++) {
            if (tiles[selected_tile_index].edges[LEFT] == u10_flip_bits(tiles[previous_tile_index].edges[RIGHT])) {
                break;
            }
            spin_tile(&tiles[selected_tile_index]);
            if (arangement == 3) {
                flip_tile(&tiles[selected_tile_index]);
            }
        }
    }
    
    // Place the rest of the tiles
    for (u16 i = square_len; i < tile_count; i++) {
        u16 previous_tile_index = aranged_tiles_indexes[i - square_len];
        u16 face_to_match = tiles[previous_tile_index].edges[BOTTOM];
        s16 selected_tile_index = faces_to_tile_idx[face_to_match][0];
        if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
            selected_tile_index = faces_to_tile_idx[face_to_match][1];
            if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
                u16 face_to_match_flipped = u10_flip_bits(face_to_match);
                selected_tile_index = faces_to_tile_idx[face_to_match_flipped][0];
                if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
                    selected_tile_index = faces_to_tile_idx[face_to_match_flipped][1];
                    if (selected_tile_index == -1 || selected_tile_index == previous_tile_index) {
                    }
                }
            }
        }
        aranged_tiles_indexes[i] = selected_tile_index;
        for (u16 arangement = 0; arangement < 8; arangement++) {
            if (tiles[selected_tile_index].edges[TOP] == u10_flip_bits(tiles[previous_tile_index].edges[BOTTOM])) {
                break;
            }
            spin_tile(&tiles[selected_tile_index]);
            if (arangement == 3) {
                flip_tile(&tiles[selected_tile_index]);
            }
        }
    }
    
    u64 id_product_part1 = 1;
    id_product_part1 *= get_tile_id(DAY20_INPUT, aranged_tiles_indexes[0]); // top left
    id_product_part1 *= get_tile_id(DAY20_INPUT, aranged_tiles_indexes[square_len - 1]); // top right
    id_product_part1 *= get_tile_id(DAY20_INPUT, aranged_tiles_indexes[tile_count - square_len]); // bottom left
    id_product_part1 *= get_tile_id(DAY20_INPUT, aranged_tiles_indexes[tile_count - 1]); // bottom right
    u64ToStr(id_product_part1, u64_result);
    
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(id_product_part1, u64_result);
    sprintf(buf, "Part 1: The product is %s", u64_result);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);

    u16 square_pixel_len = square_len * 8;
    u8 *reconstructed_image = MEM_alloc(square_len * 8 * square_len * 8);
    u8* output_ptr = reconstructed_image;
    u16 aranged_tiles_i = 0;
    for (u16 row = 0; row < square_len; row++) {
        for (u16 col = 0; col < square_len; col++) {
            u16 tile_index = aranged_tiles_indexes[aranged_tiles_i];
            draw_tile(DAY20_INPUT, output_ptr, tile_index, &tiles[tile_index], square_pixel_len);
            aranged_tiles_i++;
            output_ptr += 8;
        }
        output_ptr += 7 * square_pixel_len;
    }

    s16 roughness = part2_count(reconstructed_image, square_pixel_len);

    sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: The roughness is %d", roughness);
    drawText(buf, 1, line++);
    MEM_free(tiles);
    MEM_free(faces_to_tile_idx);
    MEM_free(aranged_tiles_indexes);
    MEM_free(reconstructed_image);
    drawText("Day 20 done, press START to go back", 1, line + 1);
}