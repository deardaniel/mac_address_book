#ifndef MAC_ADDRESS_BOOK_GROUP_H
#define MAC_ADDRESS_BOOK_GROUP_H

#include "ruby.h"
#include "AddressBook/AddressBook.h"

extern VALUE cGroup;

void init_group();
VALUE g_new_with_ABGroupRef(ABGroupRef person);

#endif