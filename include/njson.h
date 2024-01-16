#ifndef __NEROLL_NJSON_H__
#define __NEROLL_NJSON_H__

#include <string>           // string
#include <string_view>      // string_view
#include <variant>          // variant
#include <map>              // map
#include <vector>           // vector
#include <memory>           // shared_ptr
#include <utility>          // pair
#include <unordered_map>    // unordered_map

namespace neroll {

    enum class TokenType  {
        NUMBER, TRUE, FALSE, NIL, STRING, END, COMMA, COLON,
        LBRACE, RBRACE,     // { }
        LBRACKET, RBRACKET, // [ ]
    };

    const char *token_name(TokenType type);

    struct Token {
        std::string_view content;
        TokenType type;
        std::size_t lineno; // which line
        std::size_t colno;  // which column

        const char *name() const;
    };

    std::ostream &operator<<(std::ostream &os, const Token &token);

    class Lexer {
     public:
        Lexer(std::string_view json)
            : json_(json.data()), end_(json.data() + json.size()) {}

        auto next_token() -> Token;


     private:
        auto parse_true() -> Token;
        auto parse_false() -> Token;
        auto parse_null() -> Token;

        auto parse_literal(std::string_view literal, TokenType type) -> Token;

        auto parse_number() -> Token;
        auto parse_string() -> Token;

        void parse_white();

        auto match(const char *ch, TokenType type) -> Token;
        

        const char *json_;
        const char *end_;   // end of json string

        std::size_t lineno_{1}; // which line now?
        std::size_t colno_{1};  // which column now?
    };


    enum class AstType {
        OBJECT, ARRAY, STRING, INT, FLOAT, BOOLEAN, NIL
    };

    class AstNode {
     public:
        // using value_type = std::variant<std::monostate, int64_t, double,
        //     std::string, std::vector<std::shared_ptr<AstNode>>, bool,
        //     std::map<std::string, std::shared_ptr<AstNode>>>;

        AstNode(AstType type) : type_(type) {}

        AstType type() const {
            return type_;
        }
     private:
        AstType type_;
    };

    class IntNode : public AstNode {
     public:
        IntNode(int64_t value) : AstNode(AstType::INT) {
            value_ = value;
        }

        int64_t value() const {
            return value_;
        }

     private:
        int64_t value_;
    };

    class FloatNode : public AstNode {
     public:
        FloatNode(double value) : AstNode(AstType::FLOAT) {
            value_ = value;
        }

        double value() const {
            return value_;
        }
     private:
        double value_;
    };

    class ArrayNode : public AstNode {
     public:
        ArrayNode() : AstNode(AstType::ARRAY) {}

        void push_back(const std::shared_ptr<AstNode> node) {
            value_.push_back(node);
        }

        std::size_t size() const {
            return value_.size();
        }

        std::shared_ptr<AstNode> &operator[](std::size_t index);

        std::vector<std::shared_ptr<AstNode>> &value() {
            return value_;
        }

     private:
        std::vector<std::shared_ptr<AstNode>> value_;
    };

    class ObjectNode : public AstNode {
     public:

        ObjectNode() : AstNode(AstType::OBJECT) {}

        void insert(std::pair<std::string, std::shared_ptr<AstNode>> item) {
            value_.insert(item);
        }

        std::size_t size() const {
            return value_.size();
        }

        std::shared_ptr<AstNode> &at(std::string key) {
            return value_.at(key);
        }

     private:
        std::map<std::string, std::shared_ptr<AstNode>> value_;

     public:
        using value_type = decltype(value_);
    
        value_type &value() {
            return value_;
        }
    
    };

    class BooleanNode : public AstNode {
     public:
        BooleanNode(bool value) : AstNode(AstType::BOOLEAN) {
            value_ = value;
        }

        bool value() const {
            return value_;
        }
     private:
        bool value_;
    };

    class StringNode : public AstNode {
     public:
        StringNode(std::string_view value) : AstNode(AstType::STRING) {
            value_ = value;
        }

        std::string value() const {
            return value_;
        }
     private:
        std::string value_;
    };

    class NullNode : public AstNode {
     public:
        NullNode() : AstNode(AstType::NIL) {}
    };


    class Parser {
     public:
        Parser(Lexer lexer) : lexer_(lexer) {
            move();
        }
        
        auto parse() -> std::shared_ptr<AstNode>;
    
     private:
        Lexer lexer_;
        Token current_token_;

        // match literal, including true, false, null, string and number
        auto match(const Token &token) -> std::shared_ptr<AstNode>;

        void throw_error(std::string_view message, const Token &token);
        
        void move() {
            current_token_ = lexer_.next_token();
        }

        void expect(TokenType expect_type);

        auto parse_array() -> std::shared_ptr<AstNode>;
        auto parse_object() -> std::shared_ptr<AstNode>;
    };

    class Stringifier {
     public:
        Stringifier(const std::shared_ptr<AstNode> &ast) : json_ast_(ast) {
            load_config();
        }

        std::string to_html() const;
    
     private:
        std::shared_ptr<AstNode> json_ast_;
        std::shared_ptr<AstNode> config_ast_;

        std::string string_color_;
        std::string number_color_;
        std::string brace_color_;
        std::string bracket_color_;
        std::string bool_color_;
        std::string null_color_;

        void load_config();

        std::string to_html_traverse(const std::shared_ptr<AstNode> &root) const;

    };


    // class Json {
    //  public:
    //     using value_type = std::variant<int64_t, double, bool,
    //         std::map<std::string, Json>, std::vector<Json>, std::string, std::nullptr_t>;
        
    //     // for object
    //     Json &operator[](std::string_view key);
    //     const Json &operator[](std::string_view key) const;

    //     // for array
    //     Json &operator[](std::size_t index);
    //     const Json &operator[](std::size_t index) const;

    //     value_type &value();

    //  private:
    //     value_type value_;
    // };

}

#endif