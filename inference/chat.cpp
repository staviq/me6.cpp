#include "chat.hpp"

namespace Me6
{

PromptFormat::PromptFormat ( std::string filename )
    : PromptFormat ( JsonFile ( filename ).content() )
{
}

PromptFormat::PromptFormat ( json format )
    : _pf ( format )
{
}

PromptFormat::~PromptFormat() {}

std::vector< llama_token > PromptFormat::format ( std::string format, std::string role, std::string text )
{
    const auto& f = _pf["prompt_formats"][format];

    std::stringstream buf;

    buf << f[role]["prefix"];

    // auto tokens_list = llama_tokenize ( _context->context(), input["text"], false, true );

    return std::vector< llama_token >();
}

Chat::Chat() {}

Chat::~Chat() {}

} // namespace Me6
