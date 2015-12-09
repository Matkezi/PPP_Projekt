////////////////////////////////////////////////////////////////////////////////
//
//      Custom JPEG encoder
//          for FER PPP course
//
//      - Assumptions:
//          - maximum image size known
//          - image received as a RGB value list via stdin
//          - image width and height multiples of 8
//
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>

#define MAX 4096

const float RGB_to_YUV_factors[ 3 ][ 4 ] = {
    { 0.257, 0.504, 0.098, 16 },
    { -0.148, -0.291, 0.439, 128 },
    { 0.439, -0.368, -0.071, 128 }
};
const float DCT_coefficients[ 8 ][ 8 ] = {
    { .3536, .3536, .3536, .3536, .3536, .3536, .3536, .3536 },
    { .4904, .4157, .2778, .0975,-.0975,-.2778,-.4157,-.4094 },
    { .4619, .1913,-.1913,-.4619,-.4619,-.1913, .1913, .4619 },
    { .4157,-.0975,-.4904,-.2778, .2778, .4904, .0975,-.4157 },
    { .3536,-.3536,-.3536, .3536, .3536,-.3536,-.3536, .3536 },
    { .2778,-.4904, .0975, .4157,-.4157,-.0975, .4904,-.2778 },
    { .1913,-.4619, .4619,-.1913,-.1913, .4619,-.4619, .1913 },
    { .0975,-.2778, .4157,-.4904, .4904,-.4157, .2778,-.0975 }
};
const float DCT_coefficients_transp[ 8 ][ 8 ] = {
    { .3536, .4904, .4619, .4157, .3536, .2778, .1913, .0975 },
    { .3536, .4157, .1913,-.0975,-.3536,-.4904,-.4619,-.2778 },
    { .3536, .2778,-.1913,-.4904,-.3536, .0975, .4619, .4157 },
    { .3536, .0975,-.4619,-.2778, .3536, .4157,-.1913,-.4904 },
    { .3536,-.0975,-.4619, .2778, .3536,-.4157,-.1913, .4904 },
    { .3536,-.2778,-.1913, .4904,-.3536,-.0975, .4619,-.4157 },
    { .3536,-.4157, .1913, .0975,-.3536, .4904,-.4619, .2778 },
    { .3536,-.4094, .4619,-.4157, .3536,-.2778, .1913,-.0975 }
};
const short quantization_table_luminance[ 8 ][ 8 ] = {
    { 16, 11, 10, 16, 24, 40, 51, 61 },
    { 12, 12, 14, 19, 26, 58, 60, 55 },
    { 14, 13, 16, 24, 40, 57, 69, 56 },
    { 14, 17, 22, 29, 51, 87, 80, 62 },
    { 18, 22, 37, 56, 68, 109, 103, 77 },
    { 24, 35, 55, 64, 81, 104, 113, 92 },
    { 49, 64, 78, 87, 103, 121, 120, 101 },
    { 72, 92, 95, 98, 112, 100, 103, 99 }
};

short RGB_image[ MAX ][ MAX ][ 3 ];
short YUV_image[ MAX ][ MAX ][ 3 ];
short DCT_image[ MAX ][ MAX ][ 3 ];
int N, M;

void generate_YUV_image();
void quantize( int, int );
void perform_DCT( int, int );
void read_input();
void shift_values( int, int );

int main( void ){
    read_input();

    printf( "%d %d\n", N, M );

    puts( "RGB:" );
    for( int k = 0; k < 3; ++k ){
        for( int i = 0; i < N; ++i ){
            for( int j = 0; j < M; ++j )
                printf( "%6d", RGB_image[ i ][ j ][ k ] );
            puts( "" );
        }
        puts( "" );
    }

    generate_YUV_image();

    puts( "YUV:" );
    for( int k = 0; k < 3; ++k ){
        for( int i = 0; i < N; ++i ){
            for( int j = 0; j < M; ++j )
                printf( "%6d", YUV_image[ i ][ j ][ k ] );
            puts( "" );
        }
        puts( "" );
    }

    for( int i = 0; i < N; i += 8 )
        for( int j = 0; j < M; j += 8 ){
            printf( "Block (%d, %d):\n", i/8, j/8  );
            shift_values( i, j );

            for( int a = 0; a < 3; ++a ){
                for( int b = 0; b < 8; ++b ){
                    for( int c = 0; c < 8; ++c )
                        printf( "%6d", YUV_image[ i+b ][ j+c ][ a ] );
                    puts( "" );
                }
                puts( "" );
            }
            
            perform_DCT( i, j );
            quantize( i, j );
        }

    return 0;
}

void generate_YUV_image(){
    float temp;
    for( int i = 0; i < N; ++i )
        for( int j = 0; j < M; ++j )
            for( int k = 0; k < 3; ++k ){
                temp = RGB_to_YUV_factors[ k ][ 3 ];

                for( int l = 0; l < 3; ++l )
                    temp += RGB_image[ i ][ j ][ l ] * RGB_to_YUV_factors[ k ][ l ];

                YUV_image[ i ][ j ][ k ] = temp;
            }
}

void quantize( int x, int y ){

}

void perform_DCT( int x, int y ){
    float temp[ 8 ][ 8 ];
    for( int l = 0; l < 3; ++l ){
        memset( temp, 0, sizeof( temp ) );
        for( int i = 0; i < 8; ++i )
            for( int j = 0; j < 8; ++j )
                for( int k = 0; k < 8; ++k )
                    temp[ i ][ j ] += DCT_coefficients[ i ][ k ] * YUV_image[ x+k ][ y+j ][ l ];
        for( int i = 0; i < 8; ++i )
            for( int j = 0; j < 8; ++j )
                for( int k = 0; k < 8; ++k )
                    DCT_image[ x+i ][ y+j ][ l ] += temp[ i ][ k ] * DCT_coefficients_transp[ k ][ j ];
    }
}

void read_input(){
    scanf( "%d%d", &N, &M );
    for( int i = 0; i < N; ++i )
        for( int j = 0; j < M; ++j )
            for( int k = 0; k < 3; ++k )
                scanf( "%d", &RGB_image[ i ][ j ][ k ] );
}

void shift_values( int x, int y ){
    for( int i = 0; i < 8; ++i )
        for( int j = 0; j < 8; ++j )
            for( int k = 0; k < 3; ++k )
                YUV_image[ x+i ][ y+j ][ k ] -= 128;
}
