#include "cmdlargs.hpp"
#include "libs.hpp"
#include "log.hpp"
#include "public.hpp"
#include "utils.hpp"

using namespace Me6;

int main ( int argc, char** argv )
{
    CmdlArgs cmdlargs ( argc, argv );

    cmdlargs.arg ( {
        "verbose", { "-V", "--verbose" },
         "Enable detailed logging.", CmdlArgs::ReqDef::Optional, { CmdlArgs::ReqDef::Never }
    } );
    cmdlargs.arg ( {
        "srvhost",
        { "--host" },
        "IP address to listen on. Use 0.0.0.0 to listen on all "
        "interfaces. (default: 127.0.0.1)",
        CmdlArgs::ReqDef::Optional,
        { CmdlArgs::ReqDef::Required, CmdlArgs::TypDef::Text, "IP" },
        [] ( json& p )
        {
            if ( !p.contains ( "srvhost" ) )
            {
                p["srvhost"] = "127.0.0.1";
            }
          }
    } );
    cmdlargs.arg ( {
        "srvport",
        { "--port" },
        "Port to listen on. (default: 8080)",
        CmdlArgs::ReqDef::Optional,
        { CmdlArgs::ReqDef::Required, CmdlArgs::TypDef::UInteger, "PORT" },
        [] ( json& p )
        {
            if ( !p.contains ( "srvport" ) )
            {
                p["srvport"] = 8081;
            }
          }
    } );
    cmdlargs.arg ( {
        "logdisable",
        { "--log-disable" },
        "Disable logfile.",
        CmdlArgs::ReqDef::Optional,
        { CmdlArgs::ReqDef::Never, CmdlArgs::TypDef::Any, "" },
        [] ( json& p )
        {
            if ( p.contains ( "logdisable" ) && p["logdisable"].get< bool >() )
            {
                log_disable();
            }
          }
    } );
    cmdlargs.arg ( {
        "mdir",
        { "-md", "--model-directory" },
        "Directory containing .gguf models (Default: current directory).",
        CmdlArgs::ReqDef::Optional,
        { CmdlArgs::ReqDef::Required, CmdlArgs::TypDef::Text, "DIRECTORY" },
        [] ( json& p )
        {
            if ( !p.contains ( "mdir" ) )
            {
                p["mdir"] = ".";
            }
          }
    } );
    cmdlargs.arg ( {
        "model",
        { "-m", "--model" },
        "(Optional) Model file to pre-load on start.",
        CmdlArgs::ReqDef::Optional,
        { CmdlArgs::ReqDef::Required, CmdlArgs::TypDef::Text, "MODEL" },
        [] ( json& p ) {}
    } );

    try
    {
        cmdlargs.parse();
    }
    catch ( std::exception& e )
    {
        fprintf ( stderr, "Error: %s\n", e.what() );
        exit ( 1 );
    }

    const auto& params = cmdlargs.params();

    fprintf ( stderr, "CommandLine: %s\n", params.dump ( 1, '\t' ).c_str() );

#ifndef LOG_DISABLE_LOGS
    log_set_target ( log_filename_generator ( "me6", "log" ) );
    LOG_TEE ( "Log start\n" );
    log_dump_cmdline ( argc, argv );
#endif // LOG_DISABLE_LOGS

#ifdef REST_WITH_ENCRYPTION
    httplib::SSLServer srv;
#else
    httplib::Server srv;
#endif

    LLRestUuid uuid_generator;

    srv.set_pre_routing_handler (
        [] ( const Request& req, Response& res )
        {
            ME6_UNUSED ( res )
            /*LLREST_PRINT_REQUEST(req);*/
            static std::set< std::string > dbg_headers = { "Content-Type", "Content-Length", "REMOTE_ADDR", "User-Agent" };
            fprintf ( stderr, "R: %s\n\tM: '%s'\n", req.path.c_str(), req.method.c_str() );
            for ( const auto& h : req.headers )
            {
                if ( dbg_headers.find ( h.first ) != dbg_headers.end() )
                {
                    fprintf ( stderr, "\tH: '%s':'%s'\n", h.first.c_str(), h.second.c_str() );
                }
            }

            if ( req.path == "/" && me6_embedded_files.count ( "index.html" ) )
            {
                const auto& filedata = *( me6_embedded_files.find ( "index.html" ) );
                const auto& name     = filedata.first;
                const auto& data     = filedata.second;
                LOG_TEE ( "Index '%s' : (%db)\n", name.c_str(), data.len );

                res.set_content ( std::string ( ( const char* )data.data, data.len ), data.mime );

                return Server::HandlerResponse::Handled;
            }

            if ( req.method == "GET" && me6_embedded_files.find ( req.path.substr ( 1 ) ) != me6_embedded_files.end() )
            {
                const auto& filedata = *( me6_embedded_files.find ( req.path.substr ( 1 ) ) );
                const auto& name     = filedata.first;
                const auto& data     = filedata.second;

                LOG_TEE ( "File '%s' : '%s' (%db)\n", req.path.c_str(), name.c_str(), data.len );

                res.set_content ( std::string ( ( const char* )data.data, data.len ), data.mime );

                return Server::HandlerResponse::Handled;
            }

            return Server::HandlerResponse::Unhandled;
        }
    );

    // auto endpoint_index = [&]( const Request& req, Response& res )
    // { res.set_content( std::string( ( const char* )index_html, index_html_len ), "text/html" ); };
    // srv.Get( "/", endpoint_index );
    // srv.Get( "/index.html", endpoint_index );

    // for ( const auto& f : me6_embedded_files )
    // {
    //     const auto& fname = f.first;
    //     const auto& fdata = f.second;
    //     LOG_TEE ( "Ep auto %s\n", fname.c_str() );

    //     auto endpoint_auto = [&] ( const Request& req, Response& res )
    //     {
    //         LOG_TEE ( "EP: %s\n", fdata.name.c_str() );
    //         res.set_content ( std::string ( ( const char* )fdata.data, fdata.len ), fdata.mime );
    //     };
    //     srv.Get ( "/" + fdata.name, endpoint_auto );

    //     if ( fname == "index.html" )
    //     {
    //         srv.Get ( "/", endpoint_auto );
    //     }
    // }

    // std::string basedir = "public";
    // srv.set_base_dir( basedir );

    // clang-format off
    std::thread srv_t { [&](){
		try {
			if ( !srv.listen( params["srvhost"], params["srvport"] ) ) {
				LOG_TEE( "Server failed to start.\n" );
			}
		} catch ( std::exception & e ) {
			LOG_TEE( "Server: %s\n", e.what() );
		}
    } };
    // clang-format on

    if ( srv_t.joinable() )
    {
        srv_t.join();
    }

    return 0;
}