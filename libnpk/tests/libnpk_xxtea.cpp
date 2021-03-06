#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>

int libnpk_xxtea( int argc, char * argv [] )
{
    int teakey[4] = {rand(), rand(), rand(), rand()};

    NPK_PACKAGE pack;
    NPK_ENTITY entity;

    // create a pack
    CHECK( NPK_SUCCESS == npk_package_alloc( &pack, teakey ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "sample.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "zip.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_COMPRESS_ZLIB ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "xxtea.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_ENCRYPT_XXTEA ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "zipnxxtea.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_COMPRESS_ZLIB | NPK_ENTITY_ENCRYPT_XXTEA | NPK_ENTITY_REVERSE ) );
    CHECK( NPK_SUCCESS == npk_package_save( pack, "foo.npk", true ) );

    npk_package_close( pack );

    // validation
    pack = npk_package_open( "foo.npk", teakey );

    std::string entityNames[4] = { "sample.txt", "zip.txt", "xxtea.txt", "zipnxxtea.txt" };

    for( int i = 0; i < 4; ++i )
    {
        NPK_ENTITY entity = npk_package_get_entity( pack, entityNames[i].c_str() );
        CHECK( entity != NULL );

        NPK_SIZE size = npk_entity_get_size( entity );
        void* buf = malloc( size );

        CHECK( npk_entity_read( entity, buf ) );
        printf( "%d : %s\n", i, (char*)buf );
        CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample.txt" );

        free( buf );
    }

    npk_package_close( pack );

    return 0;
}
