using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace Powel  { namespace ConfigurationSystem { namespace Cli {

public value class ConfigurationGroup {
public:
	property System::String^ Name;
	property System::String^ Type;
	property int Priority;
	property System::String^ Description;

	ConfigurationGroup(System::String^ name, System::String^ type, int priority, System::String^ description)
	{
		Name = name;
		Type = type;
		Priority = priority;
		Description = description;
	}

	virtual System::String^ ToString() override {
		return System::String::Concat("[Name: '", Name, "' Type: '", Type, "' Priority: '", Priority,"' Description: '", Description, "']"); 
	}
};
public value class ConfigurationParameter {
public:
	property System::String^ Name;
	property System::String^ Type;
	property System::String^ Description;
	property System::String^ Value;

	ConfigurationParameter(System::String^ name, System::String^ value, System::String^ type, System::String^ description)
	{
		Name = name;
		Type = type;
		Description = description;
		Value = value;
	}

	virtual System::String^ ToString() override {
		return System::String::Concat("[Name: '", Name, "' Value: '", Value, "' Type: '", Type,"' Description: '", Description, "']"); 
	}
};

public interface class IConfigurationSystemManager
{

	// Produces a List with groups names.
	// Parameters:
	// filter			Search filter, supporting SQL wildcard characters: The percent sign '%' represents zero, one, or multiple characters, and the underscore '_' represents a single character.
	// Returns true on success
	bool FindGroups(System::String^ filter, [Out] List<System::String^>^% groups);

	// Produces a List of ConfigurationGroup instances. Each contain the name, type, priority and description of a group.
	// Parameters:
	// groups			Name of the groups for which to get information.
	// groupResult		The resulting configuration group informations.
	// Returns true on success
	bool GetGroups(List<System::String^>^ groups, [Out] Dictionary<System::String^, ConfigurationGroup>^% groupResult);

	// Produces a List of ConfigurationParameter instances. Each contain the name,value, type and description of a parameter.
	// Parameters:
	// group			Name of the group for which to get parameters. 
	// parameterResult	The resulting configuration parameters as a list of tuples of parameter and value on the form "parameter=value". The parameter’s value is overwritten with a new allocated list unless the function returns false.
	// Returns true on success
	bool GetParametersForGroup(System::String^ group, [Out] Dictionary<System::String^, ConfigurationParameter>^% parameterResult);

	// Produces a List with member names.
	// Parameters:
	// group			Name of the group for which to get members. 
	// membersResult	The resulting group members as a list of strings. The parameter’s value is overwritten with a new allocated list unless the function returns false.
	// Returns true on success
	bool GetMembersForGroup(System::String^ group, [Out] List<System::String^>^% membersResult);

	// Adds members to group.
	// Parameters:
	// group			Name of the group for which to add members. 
	// members			The members to add to the group as a list of strings.
	// Returns true on success
	bool AddMembersToGroup(System::String^ group, List<System::String^>^ members);

	// Removes members from group.
	// Parameters:
	// group			Name of the group for which to remove members. 
	// members			The members to remove from the group as a list of strings.
	// Returns true on success
	bool RemoveMembersFromGroup(System::String^ group, List<System::String^>^ members);

};
}}}