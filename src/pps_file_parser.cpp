#include "pps_file_parser.h"
#include "pps_rule_compiler_classes.h"
#include "pps_globals.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <list>

using std::cout;	using std::cerr;	using std::endl; using std::ifstream; using std::string;
using std::ostringstream; using std::map; using std::list; using std::size_t;

namespace Parsimonious_Production_System {

PPS_parser_exception::PPS_parser_exception(File_token_stream& input, const string& msg) :
	PPS_exception("*** Error in parsing production rule file: " + input.get_status() + msg)
	{}

PPS_file_parser::PPS_file_parser(ifstream& ifs) :
	input(ifs)
{
	load_predicate_info();
}

File_token_stream::operator bool () const
{
	return static_cast<bool> (input_file);
}

string File_token_stream::get_status() const
{
	ostringstream ss;
	ss << "line " << line_number << " position " << current_pos << endl;
	ss << line << endl;
	return ss.str();
}

void File_token_stream::display_status() const
{
	if(PPS_out)
		PPS_out << get_status();
}

void File_token_stream::reset()
{
	current_token.clear();
	current_pos = 0;
	next_pos = string::npos;
	line_number = 0;
	line.clear();
	parentheses_count = 0;
}

void File_token_stream::load_token_info() {
    // rule terms
    token_info["IF"] = "If";
    token_info["THEN"] = "Then";

    // condition terms - see actions also
    token_info["GOAL"] = "Goal";
    token_info["STEP"] = "Step";
    token_info["TAG"] = "Tag";
    token_info["VISUAL"] = "Visual";
    token_info["DETECTION"] = "Detection";
    token_info["ONSET"] = "Onset";
    token_info["OFFSET"] = "Offset";
    token_info["AUDITORY"] = "Auditory";
    token_info["NOT"] = "Not";

    // property names that might turn up in conditions
    token_info["COLOR"] = "Color";
    token_info["POSITION"] = "Position";

    // predicates
    token_info["UNIQUE"] = "Unique";
    token_info["EQUAL"] = "Equal";
    token_info["DIFFERENT"] = "Different";
    token_info["GREATER"] = "Greater";
    token_info["IF-ONLY-ONE"] = "If_only_one";
    token_info["USE-ONLY-ONE"] = "Use_only_one";
    token_info["RANDOMLY-CHOOSE-ONE"] = "Randomly_choose_one";
    token_info["LEAST"] = "Least";    // TEH 7/17/07

// built-in actions
    token_info["ADDDB"] = "Add";
    token_info["DELDB"] = "Delete";
    token_info["PREPARE"] = "Prepare";
    token_info["PERFORM"] = "Perform";
    token_info["ENABLE"] = "Enable";
    token_info["DISABLE"] = "Disable";

// motor processor terms
    token_info["MOTOR"] = "Motor";
    token_info["MODALITY"] = "Modality";
    token_info["PREPARATION"] = "Preparation";
    token_info["PROCESSOR"] = "Processor";
    token_info["EXECUTION"] = "Execution";
    token_info["FREE"] = "Free";
    token_info["BUSY"] = "Busy";
    token_info["PREVIOUS"] = "Previous";

    token_info["MANUAL"] = "Manual";
    token_info["PUNCH"] = "Punch";

    token_info["OCULAR"] = "Ocular";
    token_info["MOVE"] = "Move";

    token_info["VOCAL"] = "Vocal";
}

bool PPS_file_parser::parse_file(Compiler_rule_data_list_t& production_list, Clause_list_t& initial_memory_contents,
	map<Symbol, Geometry_Utilities::Point>& named_locations, Parameter_specification_list_t& parameters, bool output_compiler_messages)
{
	
	bool file_is_good = true;

	while(true) {
	try {
		if(input.find_token(false) != "(") {
			if(!input) {
				return file_is_good;	// eof - no more productions or definitions
				}
			else
				throw PPS_parser_exception(input, "Expecting a '(' at start of new rule or definition");
			}
		input.advance();	// skip the "("

		string token = input.find_token();
		if(token == "Define") {
			input.advance();
			parse_definitions(initial_memory_contents, named_locations, parameters, output_compiler_messages);
			}
		else {
			parse_production(production_list, output_compiler_messages);
			}
		}
	catch (PPS_parser_exception& x) {
		file_is_good = false;
		PPS_out << x.what() << endl;
		flush_past_error();
		}
	}
}

void PPS_file_parser::parse_definitions(Clause_list_t& initial_memory_contents, 
	map<Symbol, Geometry_Utilities::Point>& named_locations, Parameter_specification_list_t& parameters, 
	bool output_compiler_messages)
{
	string token = input.find_token();
	if (token == "Named_location") {
		input.advance();
		parse_named_location(named_locations, output_compiler_messages);
		}
	else if(token == "Initial_memory_contents") {
		input.advance();
		parse_initial_memory_contents(initial_memory_contents, output_compiler_messages);
		}
	else if(token == "Parameters") {
		input.advance();
		parse_parameter_list(parameters, output_compiler_messages);
		}
	else {
		throw PPS_parser_exception(input, "Expecting a valid definition type");
		}
	return;
}

void PPS_file_parser::parse_initial_memory_contents(Clause_list_t& initial_memory_contents, bool output_compiler_messages)
{
	if(initial_memory_contents.size() > 0)
		throw PPS_parser_exception(input, "Initial_memory_contents already specified");
	Clause clause;
		
	while (true) {
		parse_clause(initial_memory_contents);
		if (input.find_token() == ")") {
			input.advance();
			break;
			}
		}
	if(initial_memory_contents.size() == 0)
		throw PPS_parser_exception(input, "Initial_memory_contents must specify at least one clause");
	
	if(output_compiler_messages && PPS_out)
		PPS_out << "Initial_memory_contents with " << initial_memory_contents.size() << " clauses parsed" << endl;

}

void PPS_file_parser::parse_clause(Clause_list_t& clause_list)
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of clause");
		}
	
	input.advance();

	string token;
	Symbol_list_t terms;
	while(true) {
		token = input.find_token();
		if (token == ")") {
			input.advance();
			break;
			}
		else if (token == "(") {
			throw PPS_parser_exception(input, "Clauses cannot be nested");
			}
		terms.push_back(Symbol(token, true)); // represent numeric values if found
		input.advance();
		}
	Clause clause(terms);
	clause_list.push_back(clause);
}
void PPS_file_parser::parse_named_location(std::map<Symbol, Geometry_Utilities::Point>& named_locations, bool output_compiler_messages)
{
	string token = input.find_term();
	input.advance();
	Symbol name(token);
	std::map<Symbol, Geometry_Utilities::Point>::iterator it = named_locations.find(name);
	if(it != named_locations.end())
		throw PPS_parser_exception(input, "Named_location already defined");
	token = input.find_term();
	input.advance();
	Symbol x(token, true);	// use to construct the number
	token = input.find_term();
	input.advance();
	Symbol y(token, true);	// use to construct the number
	if(!x.has_single_numeric_value() || !y.has_single_numeric_value())
		throw PPS_parser_exception(input, "Expecting a numeric value in Named_location");
	Geometry_Utilities::Point p(x.get_numeric_value(), y.get_numeric_value());
	named_locations[name] = p;
	token = input.find_token();
	if(token != ")")
		throw PPS_parser_exception(input, "Expecting a ')' after second numeric value in Named_location");
	input.advance();
	if(output_compiler_messages && PPS_out)
		PPS_out << "Named_location " << name << " parsed" << endl;
}

void PPS_file_parser::parse_parameter_list(Parameter_specification_list_t& parameters, bool output_compiler_messages)
{
	Parameter_specification spec;
	while (true) {
		spec = parse_parameter_spec(output_compiler_messages);
		parameters.push_back(spec);
		if (input.find_token() == ")") {
			input.advance();
			break;
			}
		}
	if(parameters.size() == 0)
		throw PPS_parser_exception(input, "Define Parameters must specify at least one parameter");
	
	if(output_compiler_messages && PPS_out)
		PPS_out << "Specifications for " << parameters.size() << " parameters parsed" << endl;
}

Parameter_specification PPS_file_parser::parse_parameter_spec(bool output_compiler_messages)
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of parameter specification");
		}
	
	input.advance();
	
	Parameter_specification spec;
	string token;
	while(true) {
		token = input.find_token();
		if (token == ")") {
			input.advance();
			break;
			}
		else if (token == "(") {
			throw PPS_parser_exception(input, "Parameter specifications cannot include parenthesized items");
			}
		if(spec.processor_name.length() == 0)
			spec.processor_name = token;
		else if(spec.parameter_name.length() == 0)
			spec.parameter_name = token;
		else {
			spec.specification += token;
			spec.specification += " ";
			}
		input.advance();
		}
		
	if(output_compiler_messages && PPS_out)
		PPS_out << "Parameter " << spec.processor_name << ' ' << spec.parameter_name << ' ' << " parsed" << endl;
	return spec;
}

void PPS_file_parser::parse_production(Compiler_rule_data_list_t& production_list, bool output_compiler_messages)
{
	Compiler_rule_data production;

	string token = input.find_token();
	input.advance();
	
	production.rule_name = Symbol(token);
	// 
	token = input.find_token();
	if (token != "If")
		throw PPS_parser_exception(input, "Expecting \"If\" after rule name");
	input.advance();
	
	parse_condition_list(production);
	
	token = input.find_token();
	if (token != "Then")
		throw PPS_parser_exception(input, "Expecting \"Then\" after rule conditions");
	input.advance();

	parse_action_list(production);
	
	if (input.find_token() != ")")
		throw PPS_parser_exception(input, "Expecting a ')' at end of rule");
	input.advance();

	production_list.push_back(production);
	if(output_compiler_messages && PPS_out)
		PPS_out << "Rule " << production.rule_name << " parsed" << endl;
}

void PPS_file_parser::parse_condition_list (Compiler_rule_data& production)
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of rule conditions");
		}
	
	input.advance();
	while (true) {
		parse_condition_pattern(production);
		// stop if a ")"
		if (input.find_token() == ")") {
			input.advance();
			return;
			}
		}	
}

void PPS_file_parser::parse_condition_pattern (Compiler_rule_data& production)
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of rule condition pattern");
		}
	
	input.advance();
	
	string token = input.find_token();
	if(token == "Not") {
		input.advance();
		list_Symbol_list_t neg_pattern;
		while(true) {
			neg_pattern.push_back(parse_negated_condition_pattern());
			if (input.find_token() == ")") {
				input.advance();
				if(neg_pattern.size() > 1)
					production.negated_conjunction_patterns.push_back(neg_pattern);
				else
					production.negated_patterns.push_back(neg_pattern);
				return;
				}
			}
		}
	
	else if(is_predicate(token)) {
		Symbol_list_t pred_pattern;
		Arg_requirements requirements = get_predicate_requirements(token);
		int num_arguments_found = -1;	// the predicate name is counted
		int num_variables_found = 0;
		while(true) {
			token = input.find_token();
			if (token == ")") {
				if (num_arguments_found != requirements.num_required_args)
					throw PPS_parser_exception(input, "Invalid number of arguments found for a predicate");
				if (num_variables_found < requirements.num_required_vars)
					throw PPS_parser_exception(input, "Not enough predicate arguments are variables");
				input.advance();
				production.predicate_list.push_back(pred_pattern);
				return;
				}
			pred_pattern.push_back(Symbol(token, true));	// a number might appear
			num_arguments_found++;
			if (num_arguments_found > 0 && is_variable(Symbol(token)))
				num_variables_found++;
			input.advance();
			}
		}
	
	Symbol_list_t pattern;
	while(true) {
		token = input.find_token();
		if (token == ")") {
			input.advance();
			production.positive_patterns.push_back(pattern);
			return;
			}
		pattern.push_back(Symbol(token));
		input.advance();
		}
	return;
}

Symbol_list_t PPS_file_parser::parse_negated_condition_pattern ()
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of negated condition pattern");
		}
	
	input.advance();

	string token;
	Symbol_list_t pattern;
	bool at_first = true;
	while(true) {
		token = input.find_token();
		if (token == ")") {
			input.advance();
			return pattern;
			}
		if(at_first) {
			if(is_predicate(token)) 
				throw PPS_parser_exception(input, "A predicate may not be used inside a negated pattern");
			at_first = false;
			}
		pattern.push_back(Symbol(token));
		input.advance();
		}
}

void PPS_file_parser::parse_action_list (Compiler_rule_data& production)
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of rule actions");
		}
	
	input.advance();
	while (true) {
		parse_action(production);
		if (input.find_token() == ")") {
			input.advance();
			return;
			}
		}	
}

void PPS_file_parser::parse_action (Compiler_rule_data& production)
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of rule action");
		}
	
	input.advance();
	
	string token = input.find_token();
	
	input.advance();

	if (token == "Add") {
		Symbol_list_t pattern = parse_action_pattern();
		if (pattern.size() == 0)
			throw PPS_parser_exception(input, "Add action must be followed by a pattern");
		
		if (input.find_token() == ")") {
			input.advance();
			production.add_pattern_list.push_back(pattern);
			}
		else
			throw PPS_parser_exception(input, "Expecting a ')' after Add action pattern");
		}
	
	else if (token == "Delete") {
		Symbol_list_t pattern = parse_action_pattern();
		if (pattern.size() == 0)
			throw PPS_parser_exception(input, "Delete action must be followed by a pattern");
		
		if (input.find_token() == ")") {
			input.advance();
			production.delete_pattern_list.push_back(pattern);
			}
		else
			throw PPS_parser_exception(input, "Expecting a ')' after Delete action pattern");
		}
 	else if (token == "Add_with_probability") {
        if(!production.add_command_action_list.empty())
            // below error is misleading if there are addition add commands added in the future
			throw PPS_parser_exception(input, "Only one Add_with_probability command allowed per rule");
        Symbol_list_t add_command;
        add_command.push_back(token); // put command name in
        input.advance();
        token = input.find_token();
        if (token == ")")
            throw PPS_parser_exception(input, "Expecting additional Add_with_probability command specifications");
        Symbol prob_parameter(token);
        add_command.push_back(prob_parameter);
        input.advance();
        token = input.find_token();
        if (token == ")")
            throw PPS_parser_exception(input, "Expecting additional Add_with_probability command specifications");
        production.add_command_action_list.push_back(add_command);
        Symbol_list_t add_pattern;
        add_pattern = parse_action_pattern();
        production.add_command_pattern_list.push_back(add_pattern);
		if (input.find_token() == ")") {
            input.advance();
            }
        else
 			throw PPS_parser_exception(input, "Expecting a ')' after Add_with_probability command pattern");
  		}
    
	else if (token == "Send_to_motor") {
		Symbol_list_t command = parse_command();	// ajh & teh - 3/08
		if (command.size() == 0)
			throw PPS_parser_exception(input, "Motor command must include specifications");
		command.push_front(Symbol(token));	// ajh & teh - 3/08
		
		if (input.find_token() == ")") {
			input.advance();
			production.command_action_list.push_back(command);
			}
		else
			throw PPS_parser_exception(input, "Expecting a ')' after motor command specification");
		}
		
	else if (token == "Send_to_temporal") {
		Symbol_list_t command = parse_command();
		if (command.size() == 0)
			throw PPS_parser_exception(input, "Temporal command must include specifications");
		command.push_front(Symbol(token));
		
		if (input.find_token() == ")") {
			input.advance();
			production.command_action_list.push_back(command);
			}
		else
			throw PPS_parser_exception(input, "Expecting a ')' after temporal command specification");
		}		
		
	else if (token == "Log") {
		Symbol_list_t command = parse_command();	// ajh & teh - 3/08
		if (command.size() == 0)
			throw PPS_parser_exception(input, "Log command must have at least one argument");
		command.push_front(Symbol(token));
		if (input.find_token() == ")") {
			input.advance();
			production.command_action_list.push_back(command);
			}
		else
			throw PPS_parser_exception(input, "Expecting a ')' after Log command specification");
		}

	else
		throw PPS_parser_exception(input, "Expecting a valid action");
		
}

Symbol_list_t PPS_file_parser::parse_command ()
{
	Symbol_list_t command;

	while (true) {
		string token = input.find_token();
		if (token == "(")
			throw PPS_parser_exception(input, "Expecting only terms, not a list in a command action");
		else if (token == ")")
			break;
		command.push_back(Symbol(token));
		input.advance();
		}
	return command;
}

Symbol_list_t PPS_file_parser::parse_action_pattern ()
{
	if(input.find_token() != "(") {
		throw PPS_parser_exception(input, "Expecting a '(' at start of action pattern");
		}
	
	input.advance();
	string token;
	Symbol_list_t pattern;
	while(true) {
		token = input.find_token();
		if (token == ")") {
			input.advance();
			return pattern;
			}
		pattern.push_back(Symbol(token));
		input.advance();
		}
}

void PPS_file_parser::load_predicate_info()
{
	predicate_info["If_only_one"] = Arg_requirements(0, 0);
	predicate_info["Use_only_one"] = Arg_requirements(0, 0);
	predicate_info["Randomly_choose_one"] = Arg_requirements(0, 0);

	predicate_info["Randomly_use_each"] = Arg_requirements(1, 0);
	predicate_info["Randomly_true"] = Arg_requirements(1, 0);
	predicate_info["Least"] = Arg_requirements(1, 1);	// TEH 7/17/07
	predicate_info["Greatest"] = Arg_requirements(1, 1);	// DEK 6/24/2011

	predicate_info["Equal"] = Arg_requirements(2, 1);;
	predicate_info["Different"] = Arg_requirements(2, 1);
	predicate_info["Greater_than"]  = Arg_requirements(2, 1);
	predicate_info["Greater_than_or_equal_to"]  = Arg_requirements(2, 1);
	predicate_info["Less_than"]  = Arg_requirements(2, 1);
	predicate_info["Less_than_or_equal_to"]  = Arg_requirements(2, 1);

	predicate_info["Unique"] = Arg_requirements(2, 2);
}

PPS_file_parser::Arg_requirements PPS_file_parser::get_predicate_requirements(const string& token) const
{
	map<string, Arg_requirements>::const_iterator it = predicate_info.find(token);
	if (it != predicate_info.end())
		return it->second;
	else
		return Arg_requirements();
}

bool PPS_file_parser::is_predicate(string& token) const {
    if (predicate_info.find(token) != predicate_info.end())
        return true;
    return false;
}

string File_token_stream::find_token(bool throw_if_not_found)
{

	while (!isolate_token()) {
		getline(input_file, line);
		if (input_file) {
			line_number++;
			current_pos = 0;
			next_pos = string::npos;	// means not set
			}
		else {
			if (throw_if_not_found)
				throw PPS_parser_exception(*this, "Unexpected end of input file");
			current_pos = string::npos;
			next_pos = string::npos;
			current_token.clear();
			line.clear();
			return string();	// no token found
			}
		}
	map<string, string>::const_iterator it = token_info.find(current_token);
	if (it != token_info.end())
		return it->second;
	else
		return current_token;
}

string File_token_stream::find_term(bool throw_if_not_found)
{
	string token = find_token(throw_if_not_found);
	if(token == "(" || token == ")")
		throw PPS_parser_exception(*this, "Expected a term, not a '(' or ')'");
	return token;
}

bool File_token_stream::isolate_token()
{
	const string whitespace(" \n\r\t");
	const string terminators(" ()\n\r\t");
	
	if (current_pos == string::npos) {
		current_token.clear();
		return false;	// no more tokens on the line
		}
	
	else if (current_pos != next_pos && current_token.size() > 0) {
		return true;
		}
	current_pos = line.find_first_not_of(whitespace, current_pos);
	
	if (current_pos == string::npos) {
		current_token.clear();
		return false;	// no more tokens on the line
		}
	
	switch (line[current_pos]) {
		case ';':
			current_token.clear();
			return false;	// no more tokens on this line
		case '/': 
			if (current_pos + 1 <= line.size() && line[current_pos + 1] == '/') {
				current_token.clear();
				return false;	// no more tokens on this line
				}
			break;
		case '(':
			next_pos = current_pos + 1;
			current_token = "(";
			parentheses_count++;
			return true;
		case ')':
			next_pos = current_pos + 1;
			current_token = ")";
			parentheses_count--;
			return true;
		default:
			break;
		}
	size_t end_pos = line.find_first_of(terminators, current_pos);
	size_t length;
	if (end_pos == string::npos) {
		length = end_pos;
		next_pos = string::npos;
		}
	else {
		length = (end_pos - current_pos); // note unsigned arithmetic!
		next_pos = end_pos;
		}
		
	current_token = line.substr(current_pos, length);
	return true;
}

void File_token_stream::find_end_of_balanced_parentheses()
{
	if(!input_file)
		return;
	advance();
	while(parentheses_count > 0 && find_token(false) != string()) {
		advance();
		}
}

} // end namespace