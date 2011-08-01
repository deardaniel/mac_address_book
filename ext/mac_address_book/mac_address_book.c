#include "mac_address_book.h"
#include "address_book.h"

VALUE mMacAddressBook;

void Init_mac_address_book_ext() {
	mMacAddressBook = rb_define_module("MacAddressBook");
	
	init_address_book();
}
