#include "Preambles/Procedure/Lexer.h"

#include <algorithm>

#include "Preambles/Procedure/OperatorDefinition.h"
#include "StringUtility.h"

Preamble::Procedure::Lexer::Lexer() {
	std::sort(lexerOperators.begin(), lexerOperators.end(), [](auto a, auto b) {return a.first.size() > b.first.size(); });
}
//void Preamble::Procedure::Lexer::reset() {}


void Preamble::Procedure::Lexer::setPreambleIndex(int32_t x) {
	this->preambleIndex = x;
}
std::optional<Token> Preamble::Procedure::Lexer::lexHead(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	std::optional<uint8_t> next_ch = '\0';
	while (loc.is_good()) {
		last_ch = ch;
		ch = loc.look(0).value();
		next_ch = loc.look(1);
		if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken((Token::Type)ProcedureTokenType::colon, res);
		}
		else if (ch == ',') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken((Token::Type)ProcedureTokenType::comma, res);
		}
		else if ((ch == '(' or ch == ')')) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken(Token::Type::parenthesis, res);
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken(Token::Type::atom, res);
		}
		else {
			loc += ch;
		}
	}
	return std::nullopt;
}

std::optional<Token> Preamble::Procedure::Lexer::lexBody(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	std::optional<uint8_t> next_ch = '\0';
	bool dot = false;
	bool numberStarted = false;
	CodeLocation number(loc);

	auto returnLeftOvers = [&]() {
		auto res = loc;
		loc = loc.moveStartToEnd();
		return createToken(Token::Type::atom, res);
		};

	while (loc.is_good()) {
		last_ch = ch;
		ch = loc.look(0).value();
		next_ch = loc.look(1);
		if (loc.empty() and not isSpace(ch)) {
			for (auto rep : lexerOperators) {
				if (isCharIdentifier(loc.look(rep.first.size())) and isCharIdentifier(rep.first[rep.first.size() - 1])) continue; // if lastsymbol of operator is CharIndentifiere valible nad next is also then we have atom and not a operator ex. (let orphan = 3;)
				if (rep.first == loc.peek(rep.first.size())) {
					loc.consume(rep.first.size());
					auto res = loc;
					loc = loc.moveStartToEnd();
					return createToken((Token::Type)rep.second, res);
				}
			}
		}
		if ((isDigit(ch) or (ch == '.' and not dot))) {
			if (ch == '.') {
				dot = true;
			}
			numberStarted = true;
			loc += ch;
		}
		else if (numberStarted and (isSpace(ch) or not isDigit(ch))) {
			auto res = loc;
			loc = loc.moveStartToEnd();
			if (dot) {
				return createToken((Token::Type)ProcedureTokenType::double_literal, res);
			}
			else {
				return createToken((Token::Type)ProcedureTokenType::integer_literal, res);
			}
		}
		else if (ch == '(' or ch == ')' or ch == '[' or ch == ']') {
			if (not loc.empty()) return returnLeftOvers();

			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken(Token::Type::parenthesis, res);
		}
		else if (ch == ';') {
			if (not loc.empty()) return returnLeftOvers();

			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken((Token::Type)ProcedureTokenType::semicolon, res);
		}
		else if (ch == ',') {
			if (not loc.empty()) return returnLeftOvers();


			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken((Token::Type)ProcedureTokenType::comma, res);
		}
		else if (ch == ':') {
			if (not loc.empty()) return returnLeftOvers();

			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken((Token::Type)ProcedureTokenType::colon, res);
		}
		else if (not isSpace(ch) and (isSpace(next_ch) or not isCharIdentifier(next_ch))) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			if (res == "true" or res == "false") return createToken((Token::Type)ProcedureTokenType::bool_literal, res);
			return createToken(Token::Type::atom, res);
		}
		else {
			loc += ch;
		}
	}
	return std::nullopt;
}
std::string Preamble::Procedure::Lexer::to_string(Token::Type kind) const {
	switch ((ProcedureTokenType)kind) {
	case ProcedureTokenType::colon: return "colon";
	case ProcedureTokenType::comma: return "comma";
	case ProcedureTokenType::semicolon: return "semicolon";
	case ProcedureTokenType::keyword: return "keyword";
	case ProcedureTokenType::double_literal: return "double_literal";
	case ProcedureTokenType::integer_literal: return "integer_literal";
	case ProcedureTokenType::operator_t: return "operator_t";
	default: return "<unknown>";
	}
}
Preamble::Procedure::Lexer::~Lexer() {

}