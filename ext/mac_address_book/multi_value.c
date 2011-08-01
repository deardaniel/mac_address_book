#include "multi_value.h"
#include "helpers.h"

typedef struct {
	ABMultiValueRef multiValue;
	CFStringRef identifier;
} MultiValue;

static void free_MultiValue(MultiValue *mv) {
	CFRelease(mv->identifier);
	CFRelease(mv->multiValue);
	free(mv);
}

VALUE
mv_new_with_ABMultiValueRef_identifer(ABMultiValueRef mv, CFStringRef identifier)
{
	MultiValue *ptr = malloc(sizeof(MultiValue));
	ptr->multiValue = CFRetain(mv);
	ptr->identifier = CFRetain(identifier);

	VALUE instance = Data_Wrap_Struct(cMultiValue, 0, free_MultiValue, ptr);
	return instance;
}

static VALUE
mv_label(VALUE self)
{
	MultiValue *mv;
	Data_Get_Struct(self, MultiValue, mv);
	CFIndex index = ABMultiValueIndexForIdentifier(mv->multiValue, mv->identifier);
	CFStringRef label = ABMultiValueCopyLabelAtIndex(mv->multiValue, index);
	VALUE ret = CFStringToVALUE(label);
	CFRelease(label);
	return ret;
}

static VALUE
mv_localized_label(VALUE self)
{
	MultiValue *mv;
	Data_Get_Struct(self, MultiValue, mv);
	CFIndex index = ABMultiValueIndexForIdentifier(mv->multiValue, mv->identifier);

	CFStringRef label = ABMultiValueCopyLabelAtIndex(mv->multiValue, index);
	CFStringRef lLabel = ABCopyLocalizedPropertyOrLabel(label);

	VALUE ret = CFStringToVALUE(lLabel);

	CFRelease(lLabel);
	CFRelease(label);

	return ret;
}

static VALUE
mv_identifier(VALUE self)
{
	MultiValue *mv;
	Data_Get_Struct(self, MultiValue, mv);
	return CFStringToVALUE(mv->identifier);
}

static VALUE
mv_value(VALUE self)
{
	MultiValue *mv;
	Data_Get_Struct(self, MultiValue, mv);
	CFIndex index = ABMultiValueIndexForIdentifier(mv->multiValue, mv->identifier);

	CFTypeRef value = ABMultiValueCopyValueAtIndex(mv->multiValue, index);
	ABPropertyType type = ABMultiValuePropertyType(mv->multiValue);
	type &= ~kABMultiValueMask;
	
	VALUE finalValue = Qnil;
	if (type == kABStringProperty) {
		CFStringRef localValue = ABCopyLocalizedPropertyOrLabel(value);
		finalValue = CFStringToVALUE(localValue);
		CFRelease(localValue);
	} else {
		finalValue = CF2VALUE(value, type);
	}
	CFRelease(value);

	return finalValue;
}

static VALUE
mv_primary(VALUE self)
{
	MultiValue *mv;
	Data_Get_Struct(self, MultiValue, mv);
	CFStringRef primary = ABMultiValueCopyPrimaryIdentifier(mv->multiValue);
	return CFStringCompare(primary, mv->identifier, 0) == kCFCompareEqualTo ? Qtrue : Qfalse;
}

VALUE cMultiValue;
void init_multi_value()
{
	VALUE addressBook = rb_define_module("MacAddressBook");
	cMultiValue = rb_define_class_under(addressBook, "MultiValue", rb_cObject);

	rb_define_method(cMultiValue, "label", mv_label, 0);
	rb_define_method(cMultiValue, "localized_label", mv_localized_label, 0);
	rb_define_method(cMultiValue, "identifier", mv_identifier, 0);
	rb_define_method(cMultiValue, "value", mv_value, 0);
	rb_define_method(cMultiValue, "primary?", mv_primary, 0);
}
