#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <Windows.h>

int index = 0;
int data_size = 0;
bool complete = false;

void status( )
{
    double percent = 0;
    while ( !complete )
    {
        percent = ( static_cast< double >( index ) / data_size ) * 100;

        std::cout << "\r[" << index << "/" << data_size << "] -> ["
            << std::fixed << std::setprecision( 2 ) << percent << "%/100%]" << std::flush; // put this in another thread
    }

    Sleep( -1 );
}

int main( int argc, char* argv[ ] )
{
    SetConsoleTitleA( "file2hex" );
    if ( argc != 2 ) {
        std::cerr << "Usage: " << argv[ 0 ] << " <input_file>\n";
        MessageBoxA( 0, "Invalid usage.", "Error", 0 );
        return 1;
    }

    printf( "file2hex\n------------------------\n\n" );

    printf( "Opening file..\n" );

    std::ifstream input( argv[ 1 ], std::ios::binary );
    if ( !input ) {
        MessageBoxA( 0, "Could not open input file.", "Error", 0 );
        return 1;
    }

    printf( "Extracting.." );

    // Read entire file into a vector
    std::vector<uint8_t> data;
    uint8_t byte;
    while ( input.read( reinterpret_cast< char* >( &byte ), 1 ) ) 
    {
        data.push_back( byte );
    }

    input.close( );

    printf( " [Size: %i bytes]\n", data.size() );

    std::string output_filename = std::string( argv[ 1 ] ) + ".h";

    printf( "Creating output file.." );
    std::cout << " [" << output_filename << "]\n";
         
    std::ofstream output( output_filename );
    if ( !output ) {
        MessageBoxA( 0, "Could not create output file.", "Error", 0 );
        std::cerr << "Error: Could not create output file " << output_filename << "\n";
        return 1;
    }

    printf( "Formatting.. [Note: This might take a long time.]\n" );

    CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )status, NULL, NULL, NULL );

    // Write formatted output
    output << "//Dumped by file2hex\n\n//------------------------\n//File name: " << argv[ 1 ] << "\n//Size: " << data.size() << " bytes\n//Usage: data.data()\n//------------------------";
    output << "\n\n#include <Windows.h>\n#include <vector>\n\nstd::vector<uint8_t> data = {\n    ";

    data_size = data.size( );

    for ( size_t i = 0; i < data.size( ); ++i ) 
    {
        index = i;

        output << "0x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast< int >( data[ i ] );
        if ( i != data.size( ) - 1 )
            output << ", ";
        if ( ( i + 1 ) % 16 == 0 && i != data.size( ) - 1 )
            output << "\n    ";
    }
    output << "\n};\n";

    output.close( );

    complete = true;

    std::string msg = "File has been dumped & formatted successfully.\n\nHex file has been saved at: " + output_filename;

    MessageBoxA( 0, msg.c_str(), "Success", 0 );

    exit( 0 );
}
