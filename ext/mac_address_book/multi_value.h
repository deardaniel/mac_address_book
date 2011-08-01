#ifndef MAC_ADDRESS_BOOK_MULTI_VALUE_H
#define MAC_ADDRESS_BOOK_MULTI_VALUE_H

#include "ruby.h"
#include "AddressBook/AddressBook.h"

extern VALUE cMultiValue;

void init_multi_value();
VALUE mv_new_with_ABMultiValueRef_identifer(ABMultiValueRef mv, CFStringRef identifier);

#endif
