#include "record.h"
#include "helpers.h"
#include "multi_value.h"

static VALUE
r_getProperty(VALUE self)
{
	ABRecordRef ptr;
	Data_Get_Struct(self, void, ptr);

	VALUE ret = Qnil;
	VALUE sym = rb_funcall(rb_mKernel, rb_intern("__method__"), 0);
	VALUE property = rb_hash_aref(rb_cv_get(CLASS_OF(self), "@@properties"), sym);
	CFStringRef propertyName = CFStringCreateWithCString(kCFAllocatorDefault, RSTRING_PTR(property), kCFStringEncodingUTF8);
	if (propertyName == NULL) return Qnil;

	CFTypeRef value = ABRecordCopyValue(ptr, propertyName);
	if (value == NULL) {
		CFRelease(propertyName);
		return Qnil;
	}

	CFStringRef recordType = ABRecordCopyRecordType(ptr);
	ABPropertyType type = ABTypeOfProperty(ABGetSharedAddressBook(), recordType, propertyName);
	CFRelease(recordType);
	CFRelease(propertyName);

	ret = CF2VALUE(value, type);
	CFRelease(value);

	return ret;
}

static VALUE
r_setProperty(VALUE self, VALUE value)
{
	ABRecordRef ptr;
	Data_Get_Struct(self, void, ptr);

	VALUE ret = Qnil;
	VALUE sym = rb_funcall(rb_mKernel, rb_intern("__method__"), 0);
	const char *key = rb_id2name(SYM2ID(sym));
	
	char buffer[256];
	strncpy(buffer, key, sizeof(buffer));
	buffer[strlen(key)-1] = '\0';
	
	VALUE property = rb_hash_aref(rb_cv_get(CLASS_OF(self), "@@properties"), ID2SYM(rb_intern(buffer)));
	CFStringRef propertyName = CFStringCreateWithCString(kCFAllocatorDefault, RSTRING_PTR(property), kCFStringEncodingUTF8);
	if (propertyName == NULL) return Qnil;

	CFStringRef recordType = ABRecordCopyRecordType(ptr);
	ABPropertyType type = ABTypeOfProperty(ABGetSharedAddressBook(), recordType, propertyName);
	CFRelease(recordType);
	CFTypeRef cfValue = VALUE2CF(value, type);
	CFShow(cfValue);
	ABRecordSetValue(ptr, propertyName, cfValue);
	
	CFRelease(cfValue);
	CFRelease(propertyName);
}

void
r_setupRecordProperties(VALUE klass, CFStringRef recordType)
{
	VALUE ret = rb_hash_new();
	
	CFArrayRef properties = ABCopyArrayOfPropertiesForRecordType(ABGetSharedAddressBook(), recordType);
	
	int i;
	for (i=0; i < CFArrayGetCount(properties); i++) {
		CFStringRef property = (CFStringRef)CFArrayGetValueAtIndex(properties, i);
		if (CFStringCompare(property, CFSTR("ABPersonFlags"), 0) == kCFCompareEqualTo ||
				CFStringCompare(property, CFSTR("HomePage"), 0) == kCFCompareEqualTo) continue;
		VALUE strOrig = CFStringToVALUE(property);
		VALUE str = CFStringToVALUE(property);

		const char *regexStr1 = "([A-Z]+)([A-Z][a-z])";
		const char *regexStr2 = "([a-z\\d])([A-Z])";
		const char *regexStr3 = "([A-Z]{2,})_([A-Z])s$";
		VALUE regex1 = rb_reg_new(regexStr1, strlen(regexStr1), 0);
		VALUE regex2 = rb_reg_new(regexStr2, strlen(regexStr2), 0);
		VALUE regex3 = rb_reg_new(regexStr3, strlen(regexStr3), 0);
		VALUE sub1 = rb_str_new2("\\1_\\2");
		VALUE sub2 = rb_str_new2("\\1\\2s");

		rb_funcall(str, rb_intern("gsub!"), 2, regex1, sub1);
		rb_funcall(str, rb_intern("gsub!"), 2, regex2, sub1);
		rb_funcall(str, rb_intern("gsub!"), 2, regex3, sub2);
		rb_funcall(str, rb_intern("tr!"), 2, rb_str_new2("-"), rb_str_new2("_"));
		rb_funcall(str, rb_intern("downcase!"), 0);
		
		VALUE str2 = rb_str_plus(str, rb_str_new2("="));

		rb_hash_aset(ret, ID2SYM(rb_intern(RSTRING_PTR(str))), strOrig);

		rb_define_method(klass, RSTRING_PTR(str), r_getProperty, 0);
		rb_define_method(klass, RSTRING_PTR(str2), r_setProperty, 1);
	}
	CFRelease(properties);
	
	rb_cv_set(klass, "@@properties", ret);
}

VALUE
r_to_hash(VALUE self)
{
	int values_i(VALUE key, VALUE value, VALUE ary)
	{
	    if (key == Qundef) return ST_CONTINUE;
	    rb_ary_push(ary, value);
	    return ST_CONTINUE;
	}
	ABRecordRef record;
	Data_Get_Struct(self, void, record);

	CFStringRef recordType = ABRecordCopyRecordType(record);

	VALUE ary = rb_ary_new();
  rb_hash_foreach(rb_cv_get(CLASS_OF(self), "@@properties"), values_i, ary);

	VALUE ret = rb_hash_new();
	int i;
	for (i=0; i < RARRAY_LEN(ary); i++) {
		CFStringRef propertyName = CFStringCreateWithCString(kCFAllocatorDefault, RSTRING_PTR(RARRAY_PTR(ary)[i]), kCFStringEncodingUTF8);

		CFTypeRef value = ABRecordCopyValue(record, propertyName);
		if (value == NULL) {
			CFRelease(propertyName);
			continue;
		}
		ABPropertyType type = ABTypeOfProperty(ABGetSharedAddressBook(), recordType, propertyName);

		CFStringRef localisedName = ABCopyLocalizedPropertyOrLabel(propertyName);
		rb_hash_aset(ret, CFStringToVALUE(localisedName), CF2VALUE(value, type));

		CFRelease(localisedName);
		CFRelease(propertyName);
		CFRelease(value);
	}
	
	CFRelease(recordType);
	
	return ret;
}

static VALUE
r_dup(VALUE self)
{
	ABRecordRef record;
	Data_Get_Struct(self, void, record);
	ABRecordRef copy = ABRecordCreateCopy(record);
	VALUE new_instance = Data_Wrap_Struct(CLASS_OF(self), 0, CFRelease, copy);
	return new_instance;
}

static VALUE
r_unique_id(VALUE self)
{
	ABRecordRef ptr;
	Data_Get_Struct(self, void, ptr);
	CFStringRef name = ABRecordCopyUniqueId(ptr);
	VALUE ret = CFStringToVALUE(name);
	CFRelease(name);
	return ret;
}

VALUE mRecord;
void init_record()
{
	VALUE addressBook = rb_define_module("MacAddressBook");
	mRecord = rb_define_module_under(addressBook, "Record");
	rb_define_method(mRecord, "to_hash", r_to_hash, 0);
	rb_define_method(mRecord, "dup", r_dup, 0);
	rb_define_method(mRecord, "unique_id", r_unique_id, 0);
	init_multi_value();
	init_person();
	init_group();
}
