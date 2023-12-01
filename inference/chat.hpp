#ifndef ME6_CHAT_HPP
#define ME6_CHAT_HPP

#include "common/libs.hpp"
#include "common/utils.hpp"
#include "common/cmdlargs.hpp"

#include <common/common.h>
#include <llama.h>

namespace Me6
{

class PromptFormat
{
    public:
        PromptFormat ( std::string filename );
        PromptFormat ( json format );
        ~PromptFormat();

    public:
        std::vector< llama_token > format ( std::string format, std::string role, std::string text );

    private:
        json _pf;
};

class Chat
{
    public:
        Chat();
        ~Chat();
};

} // namespace Me6

#endif