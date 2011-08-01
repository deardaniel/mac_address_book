#ifndef MAC_ADDRESS_BOOK_HELPERS_H
#define MAC_ADDRESS_BOOK_HELPERS_H

#include "ruby.h"
#include "AddressBook/AddressBook.h"

VALUE CFStringToVALUE(CFStringRef str);
VALUE CF2VALUE(CFTypeRef value, ABPropertyType type);
CFTypeRef VALUE2CF(VALUE value, ABPropertyType type);

#endif
