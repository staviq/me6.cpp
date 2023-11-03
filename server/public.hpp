#ifndef ME6_CPP_PUBLIC_HPP
#define ME6_CPP_PUBLIC_HPP
#include "public/index.html.hpp"
#include "public/favicon.ico.hpp"
#include "public/jsoned.min.js.hpp"
#include "public/jquery.min.js.hpp"

#include <string>
#include <unordered_map>

// clang-format off
struct EmbFile{
    EmbFile(
        unsigned char * data,
        unsigned int & len,
        std::string name,
        std::string mime
    )
    :
        data(data),
        len(len),
        name(name),
        mime(mime)
    {}
    unsigned char * data;
    unsigned int & len;
    std::string name;
    std::string mime;
};

static const std::unordered_map<const std::string,const EmbFile,std::hash<std::string>> me6_embedded_files{
    {{"index.html"}, {index_html, index_html_len, "index.html", "text/html"}},
    {{"favicon.ico"}, {favicon_ico, favicon_ico_len, "favicon.ico", "image/vnd.microsoft.icon"}},
    {{"jsoned.min.js"}, {jsoned_min_js, jsoned_min_js_len, "jsoned.min.js", "application/javascript"}},
    {{"jquery.min.js"}, {jquery_min_js, jquery_min_js_len, "jquery.min.js", "application/javascript"}}
};
// clang-format on
#endif
