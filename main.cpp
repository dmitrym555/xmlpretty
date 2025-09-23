#include <iostream>

#include <string>

#include "../CommonCpp/KSUtils/KSUtil.h"
#include "../CommonCpp/KSEventLog/KSEventLog.h"

#include <format>

enum class CLState { none, tag, text };


int main( int argc, char* argv[] )
{
    std::string fname = argv[1];
    FILE* fff = fopen( fname.c_str(), "r" );

    KSFileAuto fffauto( fff );

    char buf[4096];
    int level = 0;
    CLState state = CLState::none;
    std::string tagname;
    std::string text;
    std::string out;
    bool tagWasOpened = false;

    size_t sz = 0;

    do {
        sz = fread( buf, 1, sizeof( buf ), fff );

        for ( size_t i = 0; i < sz; ++i ) {
            char chr = buf[i];
            switch ( chr ) {
                case '\n':
                case '\r':
                    continue;
                break;
                case '<':
                    if ( state == CLState::tag ) {
                        Log().E( std::format( "found < when in tag mode" ) );
                        return -1;
                    }
                    state = CLState::tag;
                    tagname = "";
                break;
                case '>':
                {
                    if ( state == CLState::text ) {
                        Log().E( std::format( "found > when in text mode" ) );
                        return -1;
                    }
                    bool startTag = (tagname[0] != '/' );

                    if ( startTag ) {
                        out += std::format( "\n{}<{}>", std::string( std::max(0,level) * 4, ' ' ),  tagname );
                        level += 1;
                        state = CLState::text;
                        text = "";
                        tagWasOpened = true;
                    }
                    else {
                        level -= 1;
                        state = CLState::none;
                        if ( tagWasOpened ) {
                            out += std::format( "{}<{}>", text, tagname );
                        }
                        else {
                            out += std::format( "\n{}<{}>", std::string( std::max(0,level) * 4, ' ' ),  tagname );
                        }
                        tagWasOpened = false;
                    }
                }
                break;
                default:
                    if ( state == CLState::tag ) {
                        tagname += chr;
                    }
                    else if ( state == CLState::text ) {
                        text += chr;
                    }
                break;
            }
        }
    } while (sz);

    fname += ".new";

    FILE* fffw = fopen ( fname.c_str(), "w" );

    KSFileAuto fffwauto( fffw );

    fwrite( out.c_str(), 1, out.length(), fffw );

    return 0;
}
