#ifndef MAC_ADDRESS_BOOK_RECORD_H
#define MAC_ADDRESS_BOOK_RECORD_H

#include "ruby.h"
#include "AddressBook/AddressBook.h"

extern VALUE mRecord;

void init_record();
void r_setupRecordProperties(VALUE klass, CFStringRef recordType);
VALUE r_to_hash(VALUE self);

#endif
