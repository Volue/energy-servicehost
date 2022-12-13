using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace Powel  { namespace ConfigurationSystem { namespace Cli {

public value class ConfigurationValue {
public:
	property System::String^ Value;
	property System::String^ Group;

	ConfigurationValue(System::String^ val, System::String^ group)
	{
		Value = val;
		Group = group;
	}

	virtual System::String^ ToString() override {
		return System::String::Concat("[Value: '", Value, "' Group: '", Group, "']"); 
	}

};

public value class Role {
public:
	property int Key;
	property System::String^ Name;
	property System::String^ User;

	Role(int key, System::String^ name, System::String^ user)
	{
		Key = key;
		Name = name;
		User = user;
	}

	virtual System::String^ ToString() override {
		return System::String::Concat("[Key: ", Key, " Name: '", Name, "' User: '", User, "']"); 
	}
};

public interface class IConfigurationSystemReader
{
	// Produces a List with tuples of parameter and value on the form "parameter=value".
	// Parameters:
	// username	    The name of the user for which to get configuration. The configuration system merely treats this as a name and does not require it to be a domain or machine user of any sort.
	// computername	The computer name for which to get configuration. The configuration system merely treats this as a name and does not require it to be a real computer of any sort.
	// progName 	The program name for which to get configuration. The configuration system merely treats this as a name and does not require it to be a real program name of any sort.
	// configResult The resulting configuration parameters as a list of tuples of parameter and value on the form "parameter=value". The parameter’s value is overwritten with a new allocated list unless the function returns false.
	// Returns true on success
	bool GetConfiguration(System::String^ username, System::String^ computername, System::String^ progName, 
		[Out] List<System::String^>^% configResult);

	// Produces a List with parameter names that are encrypted".
	// Parameters:
	// configResult The resulting parameter names as a list of parameter names. The parameter’s value is overwritten with a new allocated list unless the function returns false.
	// Returns true on success
	bool GetEncryptedParameters([Out] List<System::String^>^% configResult);

	// Produces a Dictionary with parameter name as key and ConfigurationValue-instances as value
	// where parameters referring to other parameters are resolved. The syntax for referral is the 
	// standard DOS and Windows with percent signs, e.g. “%HOME%”.
	// The following entries are inserted into configResult:
	// * USERNAME set to the parameter username
	// * COMPUTERNAME set to the parameter computername
	// Parameters: 
	// username  	The name of the user for which to get configuration. The configuration system merely treats this as a name and does not require it to be a domain or machine user of any sort.
	// computername The computer name for which to get configuration. The configuration system merely treats this as a name and does not require it to be a real computer of any sort.
	// progName  	Not used
	// configResult The resulting configuration parameters as a dictionary with parameter names as keys. The parameter’s value is overwritten with a new allocated dictionary unless the function returns false.
	// Returns true on success
	bool GetExpandedConfiguration(System::String^ username, 
		System::String^ computername, 
		System::String^ progName, 
		[Out] Dictionary<System::String^, System::String^>^% configResult);

	// Similar to the GetExpandedConfiguration method, but the role name is used to look up the associated configuration system user.
	// The following entries are inserted into configResult:
	// * USERNAME set to the parameter username (role user)
	// * COMPUTERNAME set to the parameter computername
	// Parameters: 
	// rolename  	The key for the role to retrieve information for. The configuration system merely treats this as a name and does not require it to be a domain or machine user of any sort.
	// computername The computer name for which to get configuration. The configuration system merely treats this as a name and does not require it to be a real computer of any sort.
	// progName  	Not used
	// configResult The resulting configuration parameters as a dictionary with parameter names as keys. The parameter’s value is overwritten with a new allocated dictionary unless the function returns false.
	// Returns true on success
	bool GetExpandedConfigurationByRole(int roleKey, 
		System::String^ computername, 
		System::String^ progName, 
		[Out] Dictionary<System::String^, System::String^>^% configResult);

	// Produces a Dictionary with parameter name as key and ConfigurationValue-instances as value
	// Param: configResult is assigned a new Dictionary object on success
	// Returns true on success
	bool GetConfigurationWithGroup(System::String^ username, 
		System::String^ computername, 
		System::String^ progName, 
		[Out] Dictionary<System::String^, ConfigurationValue>^% configResult);

	// Produces a Dictionary with parameter name as key and ConfigurationValue-instances as value
	// Param: configResult is assigned a new Dictionary object on success
	// Returns true on success
	bool GetConfigurationWithGroup(System::String^ username, 
		System::String^ computername, 
		System::String^ progName,
		List<System::String^>^ filter,
		[Out] Dictionary<System::String^, ConfigurationValue>^% configResult);

	// Produces a list with all configuration system users.
	// Returns true on success
	bool GetUsers([Out] List<System::String^>^% users);

	// Produces a Dictionary of role names mapped to role user names for the roles of the supplied user.
	// Param: roleToRoleCfgName is assigned a new Dictionary object on success
	// Returns true on success
	bool GetRolesForUser(System::String^ username, 
		[Out] Dictionary<System::String^, 
		System::String^>^% roleToRoleCfgName);

	// Produces a dictionary with all configuration system roles (groups with group type 'R')
	// Returns true on success
	bool GetRoles([Out] Dictionary<int,Role>^% roles);

	// Retrieves a role key from a system role name (groups with group type 'R').
	// Information is retrieved from a cache, which is updated in a timely fashion with a periodic interval.
	// Returns roleKey (-1 if not found).
	int GetRoleKeyFromCache(System::String^ roleName);

	// Produces a dictionary with all configuration system role keys and their names (groups with group type 'R')
	// Information is retrieved from a cache, which is updated in a timely fashion with a periodic interval.
	// Returns true on success
	bool GetRoleKeysFromCache([Out] Dictionary<int, System::String^>^% roles);

	// Produces a dictionary with all configuration system role names and their keys (groups with group type 'R')
	// Information is retrieved from a cache, which is updated in a timely fashion with a periodic interval.
	// Returns true on success
	bool GetRoleNamesFromCache([Out] Dictionary<System::String^, int>^% roles);

	// Returns list of Active Directory groups assigned to user.
	// Return true on success
	bool GetUserDomainGroups(System::String^ username, [Out] List<System::String^>^% groups);

};
}}}