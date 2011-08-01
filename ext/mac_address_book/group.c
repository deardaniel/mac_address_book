#include "group.h"
#include "person.h"
#include "helpers.h"

VALUE
g_new_with_ABGroupRef(ABGroupRef group)
{
	CFRetain(group);
	VALUE instance = Data_Wrap_Struct(cGroup, 0, CFRelease, group);
	return instance;
}

static VALUE
g_name(VALUE self)
{
	ABGroupRef ptr;
	Data_Get_Struct(self, void, ptr);
	CFStringRef name = ABRecordCopyValue(ptr, kABGroupNameProperty);
	VALUE ret = CFStringToVALUE(name);
	CFRelease(name);
	return ret;
}

static VALUE
g_members(VALUE self)
{
	ABGroupRef ptr;
	Data_Get_Struct(self, void, ptr);
	CFArrayRef people = ABGroupCopyArrayOfAllMembers(ptr);
	VALUE ary = rb_ary_new();
	int i;
	for (i=0; i < CFArrayGetCount(people); i++) {
		ABPersonRef person = (ABPersonRef)CFArrayGetValueAtIndex(people, i);
		if (person != NULL) {
			rb_ary_push(ary, p_new_with_ABPersonRef(person));
		}
	}
	CFRelease(people);

	return ary;
}

static VALUE
g_subgroups(VALUE self)
{
	ABGroupRef ptr;
	Data_Get_Struct(self, void, ptr);
	CFArrayRef groups = ABGroupCopyArrayOfAllSubgroups(ptr);
	VALUE ary = rb_ary_new();
	int i;
	for (i=0; i < CFArrayGetCount(groups); i++) {
		ABGroupRef group = (ABGroupRef)CFArrayGetValueAtIndex(groups, i);
		if (group != NULL) {
			rb_ary_push(ary, g_new_with_ABGroupRef(group));
		}
	}
	CFRelease(groups);

	return ary;
}

static VALUE
g_parent_groups(VALUE self)
{
	ABGroupRef ptr;
	Data_Get_Struct(self, void, ptr);
	CFArrayRef groups = ABGroupCopyParentGroups(ptr);
	VALUE ary = rb_ary_new();
	int i;
	for (i=0; i < CFArrayGetCount(groups); i++) {
		ABGroupRef group = (ABGroupRef)CFArrayGetValueAtIndex(groups, i);
		if (group != NULL) {
			rb_ary_push(ary, g_new_with_ABGroupRef(group));
		}
	}
	CFRelease(groups);

	return ary;
}

VALUE cGroup;
void init_group()
{
	VALUE addressBook = rb_define_module("MacAddressBook");
	cGroup = rb_define_class_under(addressBook, "Group", rb_cObject);
	
	VALUE record = rb_define_module_under(addressBook, "Record");
	rb_include_module(cGroup, record);
	
	rb_define_method(cGroup, "name", g_name, 0);
	rb_define_method(cGroup, "members", g_members, 0);
	rb_define_method(cGroup, "subgroups", g_subgroups, 0);
	rb_define_method(cGroup, "parent_groups", g_parent_groups, 0);
	
	r_setupRecordProperties(cGroup, kABGroupRecordType);
}
