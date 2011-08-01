#include "helpers.h"
#include "time.h"
#include "multi_value.h"

VALUE
CFStringToVALUE(CFStringRef str)
{
	if (str == NULL) {
		return Qnil;
	}
	int enc = rb_enc_find_index("UTF-8");
	const char *ptr = CFStringGetCStringPtr(str, kCFStringEncodingUTF8);
	if (ptr != NULL) {
		VALUE ret = rb_str_new2(ptr);
		rb_enc_associate_index(ret, enc);
		return ret;
	} else {
		CFIndex size;
		CFStringGetBytes(str, CFRangeMake(0, CFStringGetLength(str)), kCFStringEncodingUTF8, '?', false, NULL, 0, &size);
		UInt8 *buffer = malloc(size * sizeof(UInt8));
		
		CFStringGetBytes(str, CFRangeMake(0, CFStringGetLength(str)), kCFStringEncodingUTF8, '?', false, buffer, size, NULL);
		VALUE ret = rb_str_new(buffer, size);
		rb_enc_associate_index(ret, enc);
		
		free(buffer);
		return ret;
	}
}

static VALUE
ABMulti2VALUE(ABMultiValueRef multiValue)
{
	ABPropertyType type = ABMultiValuePropertyType(multiValue);
	if (type == kABErrorInProperty) return Qnil;
	CFIndex i;
	VALUE ary = rb_ary_new();
	for (i=0; i < ABMultiValueCount(multiValue); i++) {
		CFStringRef identifier = ABMultiValueCopyIdentifierAtIndex(multiValue, i);
		rb_ary_push(ary, mv_new_with_ABMultiValueRef_identifer(multiValue, identifier));
		CFRelease(identifier);
	}
	return ary;
}

VALUE
CF2VALUE(CFTypeRef value, ABPropertyType type)
{
	// printf(":::CF2VALUE %04x\n", (int)type);
	VALUE ret = Qnil;
	if (type & kABMultiValueMask) return ABMulti2VALUE((ABMultiValueRef)value);

	switch(type) {
		case kABStringProperty:
		{
			ret = CFStringToVALUE((CFStringRef)value);
			break;
		}
		case kABIntegerProperty:
		{
			long v;
			CFNumberGetValue(value, kCFNumberLongType, &v);
			ret = rb_fix_new(v);
			break;
		}
		case kABRealProperty:
		{
			double v;
			CFNumberGetValue(value, kCFNumberDoubleType, &v);
			ret = rb_float_new(v);
			break;
		}
		case kABDateProperty:
		{
			ret = rb_funcall(rb_cTime, rb_intern("at"), 1, INT2FIX((int)(CFDateGetAbsoluteTime(value) +  978307200.0 /*NSTimeIntervalSince1970*/)));
			break;
		}
		case kABArrayProperty:
		{
			VALUE ary = rb_ary_new();
			int i;
			for (i = 0; i < CFArrayGetCount(value); i++) {
				CFStringRef val = CFArrayGetValueAtIndex(value, i);
				rb_ary_push(ary, CFStringToVALUE(val));
			}
			ret = ary;
			break;
		}
		case kABDictionaryProperty:
		{
			CFIndex count = CFDictionaryGetCount(value);
			const void **keys = malloc(count * sizeof(void *));
			const void **values = malloc(count * sizeof(void *));
			CFDictionaryGetKeysAndValues(value, keys, values);
			
			VALUE hash = rb_hash_new();
			int i;
			for (i=0; i < count; i++) {
				rb_hash_aset(hash, CFStringToVALUE(keys[i]), CFStringToVALUE(values[i]));
			}
			ret = hash;
			
			free(keys);
			free(values);
			break;
		}
		case kABDataProperty:
		{
			ret = rb_str_new(CFDataGetBytePtr(value), CFDataGetLength(value));
			rb_enc_associate_index(ret, rb_enc_find_index("BINARY"));
			break;
		}
		case kABErrorInProperty:
		{
			rb_raise(rb_eTypeError, "error in property (kABErrorInProperty)");
			ret = Qnil;
			break;
		}
		default:
			rb_raise(rb_eTypeError, "error in property (unknown ABPropertyType)");
			ret = Qnil;
			break;
	}

	return ret;
}

static ABMultiValueRef
VALUE2ABMulti(VALUE multiValue)
{
// 	ABPropertyType type = ABMultiValuePropertyType(multiValue);
// 	if (type == kABErrorInProperty) return Qnil;
// 	// printf(":::ABMulti2VALUE %04x\n", (int)type);
// //	printf("%d entries\n", (int)ABMultiValueCount(multiValue));
// 	CFIndex i;
// 	VALUE ary = rb_ary_new();
// 	for (i=0; i < ABMultiValueCount(multiValue); i++) {
// 		CFStringRef label = ABMultiValueCopyLabelAtIndex(multiValue, i);
// 		CFStringRef lLabel = ABCopyLocalizedPropertyOrLabel(label);
// 		CFRelease(label);
// 
// 		CFTypeRef value = ABMultiValueCopyValueAtIndex(multiValue, i);
// 		type &= ~kABMultiValueMask;		
// 		
// 		VALUE finalValue = Qnil;
// 		if (type == kABStringProperty) {
// 			CFStringRef localValue = ABCopyLocalizedPropertyOrLabel(value);
// 			finalValue = CFStringToVALUE(localValue);
// 			CFRelease(localValue);
//  		} else {
// 			finalValue = CF2VALUE(value, type);
// 		}
// 
// 		CFRelease(value);
// 
// 		// printf("Label: %s, Value: %s\n", label, value);
// 		VALUE entry = rb_hash_new();
// 		rb_hash_aset(entry, ID2SYM(rb_intern("label")), CFStringToVALUE(lLabel));
// 		rb_hash_aset(entry, ID2SYM(rb_intern("value")), finalValue);
// 		rb_ary_push(ary, entry);
// 
// 		CFRelease(lLabel);
// 	}
// 	return ary;
}

CFTypeRef
VALUE2CF(VALUE value, ABPropertyType type)
{
	CFTypeRef ret = NULL;
	if (type & kABMultiValueMask) return (CFTypeRef)VALUE2ABMulti(value);

	switch(type) {
		case kABStringProperty:
		{
			ret = CFStringCreateWithBytes (
			   kCFAllocatorDefault,
			   RSTRING_PTR(value),
			   RSTRING_LEN(value),
			   kCFStringEncodingUTF8,
			   false
			);
			break;
		}
		case kABIntegerProperty:
		{
			long v = FIX2LONG(value);
			ret = CFNumberCreate(kCFAllocatorDefault, kCFNumberLongType, &v);
			break;
		}
		case kABRealProperty:
		{
			double v = rb_num2dbl(value);
			ret = CFNumberCreate(kCFAllocatorDefault, kCFNumberLongType, &v);
			break;
		}
		case kABDateProperty:
		{
			long t = FIX2INT(rb_funcall(value, rb_intern("tv_sec"), 0, NULL)) - 978307200.0; /*NSTimeIntervalSince1970*/
			ret = CFDateCreate(kCFAllocatorDefault, t);
			break;
		}
		case kABArrayProperty:
		{
			long numValues = RARRAY_LEN(value);
			CFStringRef *strings = malloc(sizeof(CFStringRef *) * numValues);
			int i;

			for (i=0; i < numValues; i++)
				strings[i] = VALUE2CF(RARRAY_PTR(value)[i], kABStringProperty);

			ret = CFArrayCreate(kCFAllocatorDefault, (const void **)strings, numValues, NULL);

			for (i=0; i < numValues; i++)
				CFRelease(strings[i]);

			break;
		}
		case kABDictionaryProperty:
		{
			CFMutableDictionaryRef ret = CFDictionaryCreateMutable(kCFAllocatorDefault, RHASH(value)->ntbl->num_entries, NULL, NULL);
			
			int each_pair_i(VALUE key, VALUE val, VALUE farg)
			{
			    if (key == Qundef) return ST_CONTINUE;
					CFDictionaryAddValue(ret, VALUE2CF(key, kABStringProperty), VALUE2CF(value, kABStringProperty));
			    return ST_CONTINUE;
			}
			
			// CFMutableDictionaryRef ret = CFDictionaryCreateMutable(kCFAllocatorDefault, RHASH(value)->ntbl->num_entries, NULL, NULL);
	    rb_hash_foreach(value, each_pair_i, Qnil);
			break;
		}
		case kABDataProperty:
		{
			ret = CFDataCreate(kCFAllocatorDefault, RSTRING_PTR(value), RSTRING_LEN(value));
			break;
		}
		case kABErrorInProperty:
		{
			rb_raise(rb_eTypeError, "error in property (unknown ABPropertyType)");
			ret = NULL;
			break;
		}
		default:
			rb_raise(rb_eTypeError, "error in property (unknown ABPropertyType)");
			ret = NULL;
			break;
	}

	return ret;
}