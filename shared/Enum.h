
#ifndef Enum_h__
#define Enum_h__

#include "LanguageConstruct.h"

struct EnumMember
{
    EnumMember(std::uint32_t ordinal, std::string const& name, std::string const& comment)
        : Offset(ordinal), ValueName(name), Value(std::to_string(ordinal)), Comment(comment) { }

    EnumMember(std::uint32_t ordinal, std::string const& value, std::string const& name, std::string const& comment)
        : Offset(ordinal), ValueName(name), Value(value), Comment(comment) { }

    std::uint32_t Offset;
    std::string ValueName;
    std::string Value;
    std::string Comment;

    bool operator<(EnumMember const& right) const { return Offset < right.Offset; }
};

class Enum : public LanguageConstruct<EnumMember>
{
    typedef LanguageConstruct<EnumMember> Base;

public:
    Enum() : _paddingAfterName(0) { }
    Enum(std::string const& name) : Base(name), _paddingAfterName(0) { }

    void SetPaddingAfterValueName(std::uint32_t padding) { _paddingAfterName = padding; }
    std::uint32_t GetPadding() const { return _paddingAfterName; }

private:
    std::uint32_t _paddingAfterName;
};

class EnumFormatter : public Formatter<Enum>
{
public:
    void ProcessMember(std::ostream& stream, Enum const& enumData, Enum::Member const& member, std::uint32_t indent) override
    {
        stream << std::string(indent + 4, ' ') << member.ValueName;

        if (enumData.GetPadding() > member.ValueName.length() + 1)
            stream << std::string(enumData.GetPadding() - member.ValueName.length() - 1, ' ');

        stream << " = " << member.Value << ',';
        if (!member.Comment.empty())
            stream << " // " << member.Comment;

        stream << std::endl;
    }
};

class CsEnum : public EnumFormatter
{
public:
    void ProcessDefinition(std::ostream& stream, Enum const& enumData, std::uint32_t indent) override
    {
        stream << std::string(indent, ' ')
            << "public enum " << enumData.GetName() << std::endl << std::string(indent, ' ')
            << '{' << std::endl;
    }

    void ProcessEnd(std::ostream& stream, Enum const& /*enumData*/, std::uint32_t indent) override
    {
        stream << std::string(indent, ' ') << "}" << std::endl;
    }
};

class CppEnum : public EnumFormatter
{
public:
    void ProcessDefinition(std::ostream& stream, Enum const& enumData, std::uint32_t indent) override
    {
        stream << std::string(indent, ' ')
            << "enum " << enumData.GetName() << std::endl << std::string(indent, ' ')
            << '{' << std::endl;
    }

    void ProcessEnd(std::ostream& stream, Enum const& /*enumData*/, std::uint32_t indent) override
    {
        stream << std::string(indent, ' ') << "};" << std::endl;
    }
};

class IdcEnum : public Formatter<Enum>
{
public:
    void ProcessDefinition(std::ostream& stream, Enum const& enumData, std::uint32_t /*indent*/) override
    {
        stream << "auto enumId;" << std::endl
               << "if ((enumId = GetEnum(\"" << enumData.GetName() << "\")) != -1)" << std::endl
               << "    DelEnum(enumId);" << std::endl << std::endl
               << "enumId = AddEnum(GetEnumQty() + 1, \"" << enumData.GetName() << "\", FF_0NUMD);" << std::endl;
    }

    void ProcessMember(std::ostream& stream, Enum const& /*enumData*/, Enum::Member const& member, std::uint32_t /*indent*/) override
    {
        stream << "AddConstEx(enumId, \"" << member.ValueName << "\", " << member.Offset << ", -1);" << std::endl;
    }

    void ProcessEnd(std::ostream& /*stream*/, Enum const& /*enumData*/, std::uint32_t /*indent*/) override
    {
    }
};

#endif // Enum_h__
