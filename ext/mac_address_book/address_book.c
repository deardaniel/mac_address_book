#include "address_book.h"
#include "AddressBook/AddressBook.h"
#include "record.h"
#include "person.h"
#include "group.h"
#include "helpers.h"

static VALUE
ab_me(VALUE self)
{
	ABAddressBookRef ptr = ABGetSharedAddressBook();
	ABPersonRef me = ABGetMe(ptr);

	return p_new_with_ABPersonRef(me);
}

static VALUE
ab_set_me(VALUE self, VALUE moi)
{
	ABPersonRef moiPerson;
	Data_Get_Struct(moi, struct __ABPerson, moiPerson);
	ABSetMe(ABGetSharedAddressBook(), moiPerson);
}

static VALUE
ab_people(VALUE self)
{
	ABAddressBookRef ptr = ABGetSharedAddressBook();
	CFArrayRef people = ABCopyArrayOfAllPeople(ptr);
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
ab_groups(VALUE self)
{
	ABAddressBookRef ptr = ABGetSharedAddressBook();
	CFArrayRef groups = ABCopyArrayOfAllGroups(ptr);
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
ab_default_country_code(VALUE self)
{
	ABAddressBookRef ptr = ABGetSharedAddressBook();
	CFStringRef cc = ABCopyDefaultCountryCode(ptr);
	VALUE ret = CFStringToVALUE(cc);
	CFRelease(cc);
	return ret;
}

static VALUE
ab_save(VALUE self)
{
	ABSave(ABGetSharedAddressBook());
}

VALUE cAddressBook;
void init_address_book()
{
	VALUE addressBook = rb_define_module("MacAddressBook");

	cAddressBook = rb_define_class_under(addressBook, "AddressBook", rb_cObject);
	rb_define_singleton_method(cAddressBook, "people", ab_people, 0);
	rb_define_singleton_method(cAddressBook, "groups", ab_groups, 0);
	rb_define_singleton_method(cAddressBook, "me", ab_me, 0);
	rb_define_singleton_method(cAddressBook, "me=", ab_set_me, 1);
	rb_define_singleton_method(cAddressBook, "default_country_code", ab_default_country_code, 0);
	rb_define_singleton_method(cAddressBook, "save", ab_save, 0);
	
	init_record();
}
