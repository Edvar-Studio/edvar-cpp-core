#pragma once

namespace edvar {
class argument_parser {
public:
    argument_parser(const container::array<edvar::string_utf16>& argument_array);
    ~argument_parser();

    bool has_option(const char* option) const;
    const char* get_option_value(const char* option, const char* default_value = nullptr) const;

    struct command_definition {
        command_definition(command_definition&& other) noexcept
            : name(edvar::move(other.name)), description(edvar::move(other.description)), parent(other.parent),
              sub_commands(edvar::move(other.sub_commands)) {
            other.parent = nullptr;
        }
        edvar::string_utf16 name;
        edvar::string_utf16 description;
        command_definition* parent = nullptr;

        container::array<struct command_definition&> sub_commands;

        command_definition& add_sub_command(command_definition in_cmd) {}
    };

    struct option_definition {
        ~option_definition() = default;
        edvar::string_utf16 name;
        edvar::string_utf16 short_name;
        edvar::string_utf16 description;
        edvar::string_utf16 default_value;
        bool requires_value = false;
    };

private:
};
} // namespace edvar