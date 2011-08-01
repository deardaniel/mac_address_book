#include "person.h"
#include "group.h"
#include "helpers.h"

VALUE
p_new_with_ABPersonRef(ABPersonRef person)
{
	CFRetain(person);
	VALUE instance = Data_Wrap_Struct(cPerson, 0, CFRelease, person);
	// rb_obj_call_init(tdata, 0, NULL);
	return instance;
}

static VALUE
p_image_data(VALUE self)
{
	ABPersonRef ptr;
	Data_Get_Struct(self, struct __ABPerson, ptr);
	CFDataRef data = ABPersonCopyImageData(ptr);
	if (data == NULL) return Qnil;

	VALUE ret = CF2VALUE(data, kABDataProperty);
	CFRelease(data);

	return ret;
}

static VALUE
p_set_image_data(VALUE self, VALUE data)
{
	ABPersonRef ptr;
	Data_Get_Struct(self, struct __ABPerson, ptr);
	CFDataRef cfData = CFDataCreate(kCFAllocatorDefault, RSTRING_PTR(data), RSTRING_LEN(data));	
	ABPersonSetImageData(ptr, cfData);
	CFRelease(cfData);
}

static VALUE
p_formatted_address(VALUE self)
{
	ABPersonRef ptr;
	Data_Get_Struct(self, struct __ABPerson, ptr);
	ABMultiValueRef addresses = ABRecordCopyValue(ptr, kABAddressProperty);
	
	CFIndex i;
	VALUE ary = rb_ary_new();
	for (i=0; i < ABMultiValueCount(addresses); i++) {
		CFStringRef label = ABMultiValueCopyLabelAtIndex(addresses, i);
		CFStringRef lLabel = ABCopyLocalizedPropertyOrLabel(label);
		CFRelease(label);

		CFDictionaryRef value = ABMultiValueCopyValueAtIndex(addresses, i);
		CFStringRef formatted = ABCreateFormattedAddressFromDictionary(ABGetSharedAddressBook(), value);
		CFRelease(value);
		VALUE finalValue = CFStringToVALUE(formatted);
		CFRelease(formatted);

		VALUE entry = rb_hash_new();
		rb_hash_aset(entry, ID2SYM(rb_intern("label")), CFStringToVALUE(lLabel));
		rb_hash_aset(entry, ID2SYM(rb_intern("value")), finalValue);
		rb_ary_push(ary, entry);

		CFRelease(lLabel);
	}
	
	CFRelease(addresses);
	return ary;
}

static VALUE
p_to_hash(VALUE self)
{
	VALUE ret = rb_call_super(0, NULL);
	rb_hash_aset(ret, rb_str_new2("Formatted Address"), p_formatted_address(self));
	return ret;
}

static VALUE
p_vcard_representation(VALUE self)
{
	ABPersonRef ptr;
	Data_Get_Struct(self, struct __ABPerson, ptr);
	CFDataRef vCard = ABPersonCopyVCardRepresentation(ptr);
	VALUE ret = CF2VALUE(vCard, kABDataProperty);
	CFRelease(vCard);
	return ret;
}

static VALUE
p_parent_groups(VALUE self)
{
	ABPersonRef ptr;
	Data_Get_Struct(self, struct __ABPerson, ptr);
	CFArrayRef groups = ABPersonCopyParentGroups(ptr);
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

VALUE cPerson;
void init_person()
{
	VALUE addressBook = rb_define_module("MacAddressBook");
	cPerson = rb_define_class_under(addressBook, "Person", rb_cObject);
	
	VALUE record = rb_define_module_under(addressBook, "Record");
	rb_include_module(cPerson, record);

	rb_define_method(cPerson, "to_hash", p_to_hash, 0);
	rb_define_method(cPerson, "image_data", p_image_data, 0);
	rb_define_method(cPerson, "image_data=", p_set_image_data, 0);
	rb_define_method(cPerson, "formatted_address", p_formatted_address, 0);
	rb_define_method(cPerson, "vcard_representation", p_vcard_representation, 0);
	rb_define_method(cPerson, "parent_groups", p_parent_groups, 0);
	
	r_setupRecordProperties(cPerson, kABPersonRecordType);
}
