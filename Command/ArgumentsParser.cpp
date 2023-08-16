#include <fstream>
#include <string>
#include <iostream>
#include "ArgumentsParser.h"
#include "../Util/Validation.h"

#define EXIT_ON_ERROR(r) if (!r) return Command::Enums::ActionType::NONE;

// ensure those functions can only be used in this cpp file
namespace
{
    void valid_or_throw(bool, int &, Command::Enums::ActionType);

    void valid_or_throw(const std::string &, Command::Enums::SZValueKey);

    boost::program_options::typed_value<bool> *get_semantic(Command::Enums::ActionType, int &);

    boost::program_options::typed_value<std::string> *get_semantic(Command::Enums::SZValueKey);

    boost::program_options::typed_value<std::string> *get_semantic(int &);
}

Command::Enums::ActionType Command::Parser::parse(int argc, char *argv[])
{
    using namespace Command::Enums;

    // region for command line
    po::options_description generic;
    generic.add_options()
            ("help", "Get command information.")
            ("version", "Get current version.");

    po::options_description db_configs;
    db_configs.add_options()
            ("db_file,db", po::value<string>()->default_value("./secret.db")->notifier(
                    [](const string &file_path) {
                        ifstream s_file(file_path);
                        bool b_exist = s_file.is_open();
                        s_file.close();
                        if (!b_exist)
                        {
                            cerr << "Please provide a .db file for operation, or make sure a default secret.db exists in the same folder of PassKeeper." << endl;
                            throw po::validation_error(po::validation_error::invalid_option_value, "db_file");
                        }
                    }
            ), "The db file for operation.")
            ("table_name,table", po::value<string>()->default_value("Password"), "The table name in the database.");

    int c_action = 0;
    po::options_description actions;
    actions.add_options()
            ("create_record,C", ::get_semantic(ActionType::CREATE, c_action), "Create a record.")
            ("read,R", ::get_semantic(ActionType::READ, c_action), "Read a record.")
            ("update_record,U", ::get_semantic(ActionType::UPDATE, c_action), "Update a record.")
            ("delete,D", ::get_semantic(ActionType::DELETE, c_action), "Delete a record.")
            ("count", ::get_semantic(ActionType::COUNT, c_action), "Count all records.");

    int c_field = 0;
    po::options_description fields;
    fields.add_options()
            ("site_name", ::get_semantic(SZValueKey::SITE_NAME), "The site name should be in upper camel case and cannot be null.")
            ("site_url", ::get_semantic(SZValueKey::SITE_URL), "The URL for site name disambiguation.")
            ("username", ::get_semantic(c_field), "Your username. If you have a particular case, like storing a CDK, do not pass this.")
            ("password", ::get_semantic(c_field), "Your password. If you have a particular case, like storing a bank card, do not pass this.")
            ("auth_text", ::get_semantic(SZValueKey::AUTH_TEXT), "The authentication text for two-step verification.")
            ("recovery_code", ::get_semantic(SZValueKey::RECOVERY_CODE), "The recovery code for account restoration.")
            ("comment", ::get_semantic(SZValueKey::COMMENT), "Some extra information.");

    po::options_description id_field;
    id_field.add_options()
            ("id", ::get_semantic(SZValueKey::ID), "The record id to locate for deletion.");

    po::options_description read_action_specific;
    read_action_specific.add_options()
            ("optimize_display,O", po::bool_switch(), "Optimize the display to show only some crucial information.")
            ("display_all,all,A", po::bool_switch(), "Display all records.");
    // endregion

    // region for prompt
    po::options_description check_options("information");
    check_options.add(generic);

    po::options_description config_options("configs");
    config_options.add(db_configs);

    po::options_description action_options("actions");
    action_options.add(actions);

    po::options_description create_options("create");
    create_options.add(id_field).add(fields);

    // we need this to ensure a new line
    po::options_description read_site_name_field;
    read_site_name_field.add_options()
            ("site_name", po::value<string>(), "The site name to locate for read.");

    po::options_description read_options("read");
    read_options.add(read_site_name_field).add(read_action_specific);

    po::options_description update_options("update");
    update_options.add(id_field).add(fields);

    po::options_description delete_options("delete");
    delete_options.add(id_field);
    // endregion

    po::options_description command_line_options;
    command_line_options.add(generic).add(db_configs).add(actions).add(fields).add(id_field).add(read_action_specific);

    // region parse input arguments to command line options
    try
    {
        po::store(po::parse_command_line(argc, argv, command_line_options), variables_map);
    }
    catch (const po::multiple_occurrences &error)
    {
        cerr << error.get_option_name() << " cannot be specified more than once" << endl;
        return ActionType::NONE;
    }
    catch (...)
    {
        cerr << "parse_command_line() raise an uncaught exception." << endl;
        return ActionType::NONE;
    }
    // endregion

    // region execute notifier
    try
    {
        po::notify(variables_map);
    }
    catch (const po::validation_error &error) // not a copy
    {
        cerr << error.get_option_name() << ": " << Command::Enums::to_string(error.kind()) << endl;
        return ActionType::NONE;
    }
    // endregion

#ifdef DEBUG_ARGUMENTSPARSER
    for (const auto &it: variables_map)
    {
        cout << it.first.c_str() << ": ";
        const auto &value = it.second.value();
        if (auto iv = boost::any_cast<int>(&value))
            cout << *iv;
        else if (auto sv = boost::any_cast<std::string>(&value))
            cout << *sv;
        else if (auto bv = boost::any_cast<bool>(&value))
            cout << *bv;
        cout << endl;
    }
#endif

    if (variables_map.count("help"))
    {
        po::options_description prompts;
        prompts.add(check_options).add(config_options).add(action_options).add(create_options).add(read_options).add(update_options).add(delete_options);
        cout << prompts << endl;
        return ActionType::NONE;
    }

    if (variables_map.count("version"))
    {
        cout << VERSION << endl;
        return ActionType::NONE;
    }

    if (c_action == 0)
    {
        cerr << "Please provide at least one action for operation." << endl;
        return ActionType::NONE;
    }

    EXIT_ON_ERROR(add_value(true, SZValueKey::DB_FILE))
    EXIT_ON_ERROR(add_value(true, SZValueKey::TABLE_NAME))

    if (variables_map.at(Command::Enums::to_string(ActionType::READ)).as<bool>())
    {
        EXIT_ON_ERROR(add_value(!add_value(BValueKey::DISPLAY_ALL), SZValueKey::SITE_NAME))
        add_value(BValueKey::OPTIMIZE_DISPLAY);
        return ActionType::READ;
    }

    if (variables_map.at(Command::Enums::to_string(ActionType::CREATE)).as<bool>())
    {
        if (c_field == 0)
        {
            cerr << "Please provide at least one username or password." << endl;
            return ActionType::NONE;
        }

        EXIT_ON_ERROR(add_value(true, SZValueKey::SITE_NAME))
        EXIT_ON_ERROR(add_value(false, SZValueKey::SITE_URL))
        EXIT_ON_ERROR(add_value(false, SZValueKey::USERNAME))
        EXIT_ON_ERROR(add_value(false, SZValueKey::PASSWORD))
        EXIT_ON_ERROR(add_value(false, SZValueKey::AUTH_TEXT))
        EXIT_ON_ERROR(add_value(false, SZValueKey::RECOVERY_CODE))
        EXIT_ON_ERROR(add_value(false, SZValueKey::COMMENT))
        return ActionType::CREATE;
    }

    if (variables_map.at(Command::Enums::to_string(ActionType::UPDATE)).as<bool>())
    {
        EXIT_ON_ERROR(add_value(true, SZValueKey::ID))
        EXIT_ON_ERROR(add_value(false, SZValueKey::SITE_NAME))
        EXIT_ON_ERROR(add_value(false, SZValueKey::SITE_URL))
        EXIT_ON_ERROR(add_value(false, SZValueKey::USERNAME))
        EXIT_ON_ERROR(add_value(false, SZValueKey::PASSWORD))
        EXIT_ON_ERROR(add_value(false, SZValueKey::AUTH_TEXT))
        EXIT_ON_ERROR(add_value(false, SZValueKey::RECOVERY_CODE))
        EXIT_ON_ERROR(add_value(false, SZValueKey::COMMENT))
        return ActionType::UPDATE;
    }

    if (variables_map.at(Command::Enums::to_string(ActionType::DELETE)).as<bool>())
    {
        EXIT_ON_ERROR(add_value(true, SZValueKey::ID))
        return ActionType::DELETE;
    }

    if (variables_map.at(Command::Enums::to_string(ActionType::COUNT)).as<bool>())
    {
        return ActionType::COUNT;
    }

    throw runtime_error("Unexpected Behaviour: WHY ARE YOU FALLING INTO THIS LINE?");
}

bool Command::Parser::add_value(bool required, Command::Enums::SZValueKey key)
{
    string column_key = Command::Enums::to_string(key);
    bool has_value = variables_map.count(column_key);
    if (required && !has_value)
    {
        cerr << column_key << ": at least one value required" << endl;
        return false;
    }
    szkv_pair.insert({ key, has_value ? variables_map.at(column_key).as<string>() : "" });
    return true;
}

bool Command::Parser::add_value(Command::Enums::BValueKey key)
{
    bool value = variables_map.at(Command::Enums::to_string(key)).as<bool>();
    bkv_pair.insert({ key, value });
    return value;
}

const SZKV_PAIR &Command::Parser::get_szkv_pair()
{
    return szkv_pair;
}

const BKV_PAIR &Command::Parser::get_bkv_pair()
{
    return bkv_pair;
}

Command::Parser::Parser()
        : szkv_pair(), bkv_pair(), variables_map()
{
}

// region implementation of internal linkage functions
namespace
{
    void valid_or_throw(bool check, int &c_action, Command::Enums::ActionType action)
    {
        if (check && ++c_action - 1)
        {
            std::cerr << "Only one action is allowed." << std::endl;
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option, Command::Enums::to_string(action));
        }
    }

    void valid_or_throw(const std::string &value, Command::Enums::SZValueKey key)
    {
        if (Validation::is_blank(value))
        {
            std::cerr << "The value you have provided is empty or blank." << std::endl;
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value, to_string(key));
        }
    }

    boost::program_options::typed_value<bool> *get_semantic(Command::Enums::ActionType action, int &c_action)
    {
        // since the function returned value is created on heap, we can do this safely.
        return boost::program_options::bool_switch()->notifier([&c_action, &action](bool check) { ::valid_or_throw(check, c_action, action); });
    }

    boost::program_options::typed_value<std::string> *get_semantic(Command::Enums::SZValueKey field)
    {
        return boost::program_options::value<std::string>()->notifier([&field](const std::string &value) { ::valid_or_throw(value, field); });
    }

    boost::program_options::typed_value<std::string> *get_semantic(int &c_field)
    {
        return boost::program_options::value<std::string>()->notifier([&c_field](const std::string &value) { !Validation::is_blank(value) && c_field++; });
    }
}
// endregion

std::string Command::Enums::to_string(Command::Enums::ActionType action)
{
    switch (action)
    {
        case ActionType::CREATE:
            return "create_record";
        case ActionType::READ:
            return "read";
        case ActionType::UPDATE:
            return "update_record";
        case ActionType::DELETE:
            return "delete";
        case ActionType::COUNT:
            return "count";
        default:
            throw runtime_error("ActionType out of range");
    }
}

std::string Command::Enums::to_string(Command::Enums::SZValueKey key)
{
    switch (key)
    {
        case SZValueKey::ID:
            return "id";
        case SZValueKey::SITE_NAME:
            return "site_name";
        case SZValueKey::SITE_URL:
            return "site_url";
        case SZValueKey::USERNAME:
            return "username";
        case SZValueKey::PASSWORD:
            return "password";
        case SZValueKey::AUTH_TEXT:
            return "auth_text";
        case SZValueKey::RECOVERY_CODE:
            return "recovery_code";
        case SZValueKey::COMMENT:
            return "comment";
        case SZValueKey::DB_FILE:
            return "db_file";
        case SZValueKey::TABLE_NAME:
            return "table_name";
        default:
            throw runtime_error("ColumnKey out of range");
    }
}

std::string Command::Enums::to_string(Command::Enums::BValueKey key)
{
    switch (key)
    {
        case BValueKey::OPTIMIZE_DISPLAY:
            return "optimize_display";
        case BValueKey::DISPLAY_ALL:
            return "display_all";
        default:
            throw runtime_error("BValueKey out of range");
    }
}

std::string Command::Enums::to_string(boost::program_options::validation_error::kind_t kind)
{
    namespace po = boost::program_options;

    switch (kind)
    {
        case po::validation_error::invalid_option_value:
            return "invalid option value";
        case po::validation_error::at_least_one_value_required:
            return "at least one value required";
        case po::validation_error::invalid_bool_value:
            return "invalid bool value";
        case po::validation_error::multiple_values_not_allowed:
            return "multiple values not allowed";
        case po::validation_error::invalid_option:
            return "invalid option";
        default:
            throw std::runtime_error("ErrorKind out of range");
    }
}