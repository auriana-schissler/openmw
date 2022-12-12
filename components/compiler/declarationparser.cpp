#include "declarationparser.hpp"

#include <string>

#include <components/compiler/parser.hpp>
#include <components/compiler/tokenloc.hpp>
#include <components/misc/strings/lower.hpp>

#include "errorhandler.hpp"
#include "locals.hpp"
#include "scanner.hpp"
#include "skipparser.hpp"

namespace Compiler
{
    class Context;
}

Compiler::DeclarationParser::DeclarationParser(ErrorHandler& errorHandler, const Context& context, Locals& locals)
    : Parser(errorHandler, context)
    , mLocals(locals)
    , mState(State_Begin)
    , mType(0)
{
}

bool Compiler::DeclarationParser::parseName(const std::string& name, const TokenLoc& loc, Scanner& scanner)
{
    if (mState == State_Name)
    {
        std::string name2 = ::Misc::StringUtils::lowerCase(name);

        char type = mLocals.getType(name2);

        if (type != ' ')
            getErrorHandler().warning("Local variable re-declaration", loc);
        else
            mLocals.declare(mType, name2);

        mState = State_End;
        return true;
    }
    else if (mState == State_End)
    {
        getErrorHandler().warning("Extra text after local variable declaration", loc);
        SkipParser skip(getErrorHandler(), getContext());
        scanner.scan(skip);
        return false;
    }

    return Parser::parseName(name, loc, scanner);
}

bool Compiler::DeclarationParser::parseKeyword(int keyword, const TokenLoc& loc, Scanner& scanner)
{
    if (mState == State_Begin)
    {
        switch (keyword)
        {
            case Scanner::K_short:
                mType = 's';
                break;
            case Scanner::K_long:
                mType = 'l';
                break;
            case Scanner::K_float:
                mType = 'f';
                break;
            default:
                mType = 0;
        }

        if (mType)
        {
            mState = State_Name;
            return true;
        }
    }
    else if (mState == State_Name)
    {
        // allow keywords to be used as local variable names. MW script compiler, you suck!
        return parseName(loc.mLiteral, loc, scanner);
    }
    else if (mState == State_End)
    {
        getErrorHandler().warning("Extra text after local variable declaration", loc);
        SkipParser skip(getErrorHandler(), getContext());
        scanner.scan(skip);
        return false;
    }

    return Parser::parseKeyword(keyword, loc, scanner);
}

bool Compiler::DeclarationParser::parseSpecial(int code, const TokenLoc& loc, Scanner& scanner)
{
    if (mState == State_End)
    {
        if (code != Scanner::S_newline)
        {
            getErrorHandler().warning("Extra text after local variable declaration", loc);
            SkipParser skip(getErrorHandler(), getContext());
            scanner.scan(skip);
        }
        return false;
    }

    return Parser::parseSpecial(code, loc, scanner);
}

bool Compiler::DeclarationParser::parseInt(int value, const TokenLoc& loc, Scanner& scanner)
{
    if (mState == State_Name)
    {
        // Allow integers to be used as variable names
        return parseName(loc.mLiteral, loc, scanner);
    }
    return Parser::parseInt(value, loc, scanner);
}

void Compiler::DeclarationParser::reset()
{
    mState = State_Begin;
}
