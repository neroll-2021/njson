#include "njson.h"

#include <stdexcept>    // runtime_error
#include <format>       // format
#include <iostream>     // ostream
#include <algorithm>    // any_of
#include <ranges>
#include <charconv>     // from_chars

auto neroll::token_name(TokenType type) -> const char * {
    switch (type) {
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::END:
            return "EOF";
        case TokenType::FALSE:
            return "FALSE";
        case TokenType::LBRACE:
            return "LBRACE";
        case TokenType::LBRACKET:
            return "LBRACKET";
        case TokenType::NIL:
            return "NULL";
        case TokenType::NUMBER:
            return "NUMBER";
        case TokenType::RBRACE:
            return "RBRACE";
        case TokenType::RBRACKET:
            return "RBRACKET";
        case TokenType::STRING:
            return "STRING";
        case TokenType::TRUE:
            return "TRUE";
        case TokenType::COLON:
            return "COLON";
        default:
            throw std::runtime_error("unknown token type");
    }
}

auto neroll::Token::name() const -> const char * {
    return token_name(type);
}

std::ostream &neroll::operator<<(std::ostream &os, const Token &token) {
    os << std::format("<{}, {}>", token.content, token.name());
    return os;
}

auto neroll::Lexer::parse_white() -> void {
    while (json_ < end_) {
        switch (*json_) {
            case ' ':
            case '\t':
                colno_++;
                break;
            case '\r':
                colno_ = 1;
                break;
            case '\n':
                colno_ = 1;
                lineno_++;
                break;
            default:
                return;
        }
        json_++;
    }
}

auto neroll::Lexer::parse_literal(std::string_view literal, TokenType type) -> Token {
    std::size_t len = 0;
    while (json_ < end_ && (std::isalnum(*json_) || *json_ == '_')) {
        len++;
        json_++;
    }
    std::string_view word(json_ - len, len);
    if (word != literal) {
        throw std::runtime_error(std::format("error: line {}, column {}: "
            "unknow indentifier {}, do you mean '{}'?", lineno_, colno_, word, literal));
    }
    colno_ += literal.size();
    return Token{word, type, lineno_, colno_};
}

auto neroll::Lexer::parse_true() -> Token {
    return parse_literal("true", TokenType::TRUE);
}

auto neroll::Lexer::parse_false() -> Token {
    return parse_literal("false", TokenType::FALSE);
}

auto neroll::Lexer::parse_null() -> Token {
    return parse_literal("null", TokenType::NIL);
}

auto neroll::Lexer::parse_number() -> Token {
    int state = 0;
    const char *start_pos = json_;
    bool complete = false;
    while (json_ < end_) {
        switch (state) {
            case 0:
                switch (*json_) {
                    case '-':
                        state = 2;
                        break;
                    case '0':
                        state = 1;
                        break;
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        state = 3;
                        break;
                    default:
                        throw std::runtime_error(std::format("error: line {}, column {}: "
                            "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 1:
                switch (*json_) {
                    case '.':
                        state = 4;
                        break;
                    default:
                        complete = true;
                        break;
                        // throw std::runtime_error(std::format("error: line {}, column {}: "
                        //     "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 2:
                switch (*json_) {
                    case '0':
                        state = 1;
                        break;
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        state = 3;
                        break;
                    default:
                        throw std::runtime_error(std::format("error: line {}, column {}: "
                            "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 3:
                switch (*json_) {
                    case 'e':
                    case 'E':
                        state = 6;
                        break;
                    case '.':
                        state = 4;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        break;
                    default:
                        complete = true;
                        break;
                        // throw std::runtime_error(std::format("error: line {}, column {}: "
                        //     "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 4:
                switch (*json_) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        state = 5;
                        break;
                    default:
                        throw std::runtime_error(std::format("error: line {}, column {}: "
                            "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 5:
                switch (*json_) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        break;
                    case 'e':
                    case 'E':
                        state = 6;
                        break;
                    default:
                        complete = true;
                        break;
                        // throw std::runtime_error(std::format("error: line {}, column {}: "
                        //     "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 6:
                switch (*json_) {
                    case '+':
                    case '-':
                        state = 7;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        state = 8;
                        break;
                    default:
                        throw std::runtime_error(std::format("error: line {}, column {}: "
                            "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 7:
                switch (*json_) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        state = 8;
                        break;
                    default:
                        throw std::runtime_error(std::format("error: line {}, column {}: "
                            "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            case 8:
                switch (*json_) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        break;
                    default:
                        complete = true;
                        break;
                        // throw std::runtime_error(std::format("error: line {}, column {}: "
                        //     "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
                }
                break;
            default:
                throw std::runtime_error(std::format("[fatal] error: line {}, column {}: "
                            "invalid number {}", lineno_, colno_, std::string_view(start_pos, json_ - start_pos + 1)));
        }
        if (complete)
            break;
        json_++;
    }
    std::string_view number_str = std::string_view(start_pos, json_ - start_pos);
    colno_ += number_str.size();
    return {number_str, neroll::TokenType::NUMBER, lineno_, colno_};
}

auto neroll::Lexer::parse_string() -> Token {
    int state = 0;
    const char *start_pos = json_;
    bool complete = false;
    while (json_ < end_) {
        switch (state) {
            case 0:
                switch (*json_) {
                    case '\"':
                        state = 1;
                        break;
                    default:
                        throw std::runtime_error(std::format("error: line {}, column {}: "
                            "string should begin with \"", lineno_, colno_));
                }
                break;
            case 1:
                switch (*json_) {
                    case '\\':
                        state = 2;
                        break;
                    case '\"':
                        state = 3;
                        break;
                    // case '\n':
                    //     throw std::runtime_error(std::format("error: line {}, column {}: "
                    //         "string cannot contain \\n", lineno_, colno_));
                    default:
                        if (*json_ < 20) {
                            throw std::runtime_error(std::format("error: line {}, column {}: "
                                "invalid string character", lineno_, colno_));
                        }
                        break;
                }
                break;
            case 2:
                switch (*json_) {
                    case '\\':
                    case '/':
                    case 'b':
                    case 'f':
                    case 'r':
                    case 'n':
                    case 't':
                    case '\"':
                        state = 1;
                        break;
                    case 'u':
                        throw std::runtime_error("unicode not support yet");
                    default:
                        throw std::runtime_error(std::format("invalid escape character: \\{}", *json_));
                }
                break;
            case 3:
                complete = true;
                break;
            default:
                throw std::runtime_error("switch error, this should not happen");
        }
        if (complete)
            break;
        json_++;
        colno_++;
    }
    auto string = std::string_view(start_pos, json_ - start_pos);
    return {string, TokenType::STRING, lineno_, colno_};
}

auto neroll::Lexer::match(const char *ch, TokenType type) -> Token {
    if (*json_ != *ch) {
        throw std::runtime_error(std::format("error: line {}, column {}:"
            "expect {}, get {}", lineno_, colno_, ch, *json_));
    }
    json_++;
    colno_++;
    return {ch, type, lineno_, colno_};
}

auto neroll::Lexer::next_token() -> Token {
    parse_white();
    if (json_ >= end_)
        return {"EOF", TokenType::END, lineno_, colno_};
    switch (*json_) {
        case 't':
            return parse_true();
        case 'f':
            return parse_false();
        case '\"':
            return parse_string();
        case 'n':
            return parse_null();
        case '{':
            return match("{", TokenType::LBRACE);
        case '}':
            return match("}", TokenType::RBRACE);
        case '[':
            return match("[", TokenType::LBRACKET);
        case ']':
            return match("]", TokenType::RBRACKET);
        case ',':
            return match(",", TokenType::COMMA);
        case ':':
            return match(":", TokenType::COLON);
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            return parse_number();
        default:
            throw std::runtime_error(std::format("error: line {}, column {}: invalid token", lineno_, colno_));
    }
}

void neroll::Parser::throw_error(std::string_view message, const Token &token) {
    auto line = token.lineno;
    auto column = token.colno;
    throw std::runtime_error(std::format("error: line {}, column {}: {}", line, column, message));
}

auto neroll::Parser::parse() -> std::shared_ptr<AstNode> {
    switch (current_token_.type) {
        case TokenType::LBRACE:
            return parse_object();
        case TokenType::LBRACKET:
            return parse_array();
        case TokenType::STRING:
            return std::make_shared<StringNode>(current_token_.content);
        case TokenType::NUMBER:
            return match(current_token_);
        case TokenType::TRUE:
        case TokenType::FALSE:
            return match(current_token_);
        case TokenType::NIL:
            return match(current_token_);
        default:
            throw std::runtime_error(std::format("parse: invalid token type: {}", token_name(current_token_.type)));
    }
}

auto neroll::Parser::match(const Token &token) -> std::shared_ptr<AstNode> {
    switch (token.type) {
        case TokenType::TRUE:
            return std::make_shared<BooleanNode>(true);
        case TokenType::FALSE:
            return std::make_shared<BooleanNode>(false);
        case TokenType::NIL:
            return std::make_shared<NullNode>();
        case TokenType::STRING:
            return std::make_shared<StringNode>(std::string{token.content});
        case TokenType::NUMBER: {
            int is_float = std::ranges::any_of(token.content, [](char  ch) {
                return ch == '.' || ch == 'e' || ch == 'E';
            });
            if (is_float) {
                double value;
                auto [ptr, errc] = std::from_chars(token.content.begin(), token.content.end(), value);
                if (errc == std::errc::invalid_argument)
                    throw_error("invalid number", token);
                if (errc == std::errc::result_out_of_range)
                    throw_error("number out of range", token);
                return std::make_shared<FloatNode>(value);
            } else {
                int64_t value;
                auto [ptr, errc] = std::from_chars(token.content.begin(), token.content.end(), value);
                if (errc == std::errc::invalid_argument)
                    throw_error("invalid number", token);
                if (errc == std::errc::result_out_of_range)
                    throw_error("number out of range", token);
                return std::make_shared<IntNode>(value);
            }
        }
        default:
            throw std::runtime_error("invalid token type");
    }
    return nullptr;
}

void neroll::Parser::expect(TokenType expect_type) {
    if (current_token_.type != expect_type)
        throw_error(std::format("unexpect token {}, expect {}",
            current_token_.content, token_name(expect_type)), current_token_);
}

auto neroll::Parser::parse_array() -> std::shared_ptr<AstNode> {
    move();
    auto array = std::make_shared<ArrayNode>();
    if (current_token_.type == TokenType::RBRACKET)
        return array;
    while (true) {
        std::shared_ptr<AstNode> value = parse();
        array->push_back(value);
        move();

        if (current_token_.type == TokenType::COMMA) {
            move();
        } else if (current_token_.type == TokenType::RBRACKET) {
            return array;
        } else {
            throw_error("missing comma or right bracket when parsing array", current_token_);
        }
    }
    return array;
}

auto neroll::Parser::parse_object() -> std::shared_ptr<AstNode> {
    move();
    auto object = std::make_shared<ObjectNode>();
    if (current_token_.type == TokenType::RBRACE)
        return object;
    while (true) {
        auto key_node = parse();
        if (key_node->type() != AstType::STRING)
            throw_error("object key should be a string", current_token_);
        auto key = std::static_pointer_cast<StringNode>(key_node)->value();
        move();
        if (current_token_.type != TokenType::COLON)
            throw_error("expect colon after key", current_token_);
        move();
        
        auto value = parse();
        object->insert({key, value});

        move();
        if (current_token_.type == TokenType::COMMA) {
            move();
        } else if (current_token_.type == TokenType::RBRACE) {
            return object;
        } else {
            throw_error("missing comma or right brace when parsing object", current_token_);
        }
    }
}

auto neroll::ArrayNode::operator[](std::size_t index) -> std::shared_ptr<AstNode> & {
    return value_[index];
}