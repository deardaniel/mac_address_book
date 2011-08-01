#ifndef MAC_ADDRESS_BOOK_PERSON_H
#define MAC_ADDRESS_BOOK_PERSON_H

#include "ruby.h"
#include "AddressBook/AddressBook.h"

extern VALUE cPerson;

void init_person();
VALUE p_new_with_ABPersonRef(ABPersonRef person);

#endif
