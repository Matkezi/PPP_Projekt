#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int N = 8, M = 8;
const int FLATNESS = 25;

short p[ 1024 ][ 1024 ][ 3 ];

short next_random( short last ){
    short sum = 0;
    for( int i = 0; i < FLATNESS; ++i )
        sum += rand() % 254;
    sum /= FLATNESS;
    short res = last + ( 127 - sum );
    if( res < 0 ) res = 0;
    if( res > 255 ) res = 255;
    return res;
}

int main(){
    srand( time( 0 ) );
    printf( "%d %d\n", N, M );
    p[ 0 ][ 0 ][ 0 ] = p[ 0 ][ 0 ][ 1 ] = p[ 0 ][ 0 ][ 2 ] = 127;
    for( int i = 0; i < N; ++i )
        for( int j = 0; j < M; ++j ){
            for( int k = 0; k < 3; ++k ){
                p[ i ][ j ][ k ] = next_random( j > 0 ? p[ i ][ j-1 ][ k ] : i > 0 ? p[ i-1 ][ j ][ k ] : 127 );
                printf( "%d ", p[ i ][ j ][ k ] );
            }
            puts( "" );
        }
}
